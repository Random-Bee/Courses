#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void grid(int nx, double xst, double xen, double *x, double *dx)
{
  int i;
  
  *dx = (xen-xst)/(double)(nx-1);

  for(i=0; i<nx; i++)
    x[i] = (double)i * (*dx); // ensure x[0] == 0.0 and x[nx-1] == 1.0
}

void enforce_bcs(int nx, int ny, double *x, double *y, double **T)
{
  int i, j;

  // left and right ends
  for(j=0; j<ny; j++)
  {
    T[0][j] = 0.0;    T[nx-1][j] = 0.0;
  }

  // top and bottom ends
  for(i=0; i<nx; i++)
  {
    T[i][0] = 0.0;    T[i][ny-1] = 0.0;
  }
}

void set_initial_condition(int nx, int ny, double *x, double *y, double **T, double dx, double dy)
{
  int i, j;
  double del=1.0;

  for(i=0; i<nx; i++)
    for(j=0; j<ny; j++)
    {
        T[i][j] = 0.25 * (tanh((x[i]-0.4)/(del*dx)) - tanh((x[i]-0.6)/(del*dx))) 
                       * (tanh((y[j]-0.4)/(del*dy)) - tanh((y[j]-0.6)/(del*dy)));
    //printf("\n%d %lf %lf %lf %lf", i, x[i], tanh((x[i]-0.45)/(del*dx)), tanh((x[i]-0.65)/(del*dx)), T[i]);
    }

  enforce_bcs(nx,ny,x,y,T); //ensure BCs are satisfied at t = 0
}

void timestep_FwdEuler(int nx, int ny, double dt, double dx, double dy, double kdiff, double *x, double *y, double **T, double **rhs)
{

  int i,j;
  double dxsq = dx*dx, dysq = dy*dy;

  // (Forward) Euler scheme
  for(i=1; i<nx-1; i++)
   for(j=1; j<ny-1; j++)
     rhs[i][j] = kdiff*(T[i+1][j]+T[i-1][j]-2.0*T[i][j])/dxsq +
           kdiff*(T[i][j+1]+T[i][j-1]-2.0*T[i][j])/dysq ;  // compute rhs

  for(i=1; i<nx-1; i++)
   for(j=1; j<ny-1; j++)
     T[i][j] = T[i][j] + dt*rhs[i][j];                           // update T^(it+1)[i]

  // set Dirichlet BCs
  enforce_bcs(nx,ny,x,y,T);

}

void output_soln(int nx, int ny, int it, double tcurr, double *x, double *y, double **T)
{
  int i,j;
  FILE* fp;
  char fname[100];

  sprintf(fname, "T_x_y_%06d.dat", it);
  //printf("\n%s\n", fname);

  fp = fopen(fname, "w");
  for(i=0; i<nx; i++)
   for(j=0; j<ny; j++)
      fprintf(fp, "%lf %lf %lf\n", x[i], y[j], T[i][j]);
  fclose(fp);

  printf("Done writing solution for time step = %d\n", it);
}

int main()
{

  int nx, ny;
  double *x, *y, **T, **rhs, tst, ten, xst, xen, yst, yen, dx, dy, dt, tcurr, kdiff;
  double min_dx_dy;
  int i, it, num_time_steps, it_print;
  FILE* fp;  

  // read inputs
  fp = fopen("input2d.in", "r");
  fscanf(fp, "%d %d\n", &nx, &ny);
  fscanf(fp, "%lf %lf %lf %lf\n", &xst, &xen, &yst, &yen);
  fscanf(fp, "%lf %lf\n", &tst, &ten);
  fscanf(fp, "%lf\n", &kdiff);
  fclose(fp);

  printf("Inputs are: %d %lf %lf %lf %lf %lf\n", nx, xst, xen, tst, ten, kdiff);
  printf("Inputs are: %d %lf %lf\n", ny, yst, yen);

  x = (double *)malloc(nx*sizeof(double));
  y = (double *)malloc(ny*sizeof(double));
  T = (double **)malloc(nx*sizeof(double *));
  for(i=0; i<nx; i++)
    T[i] = (double *)malloc(ny*sizeof(double));
  rhs = (double **)malloc(nx*sizeof(double *));
  for(i=0; i<nx; i++)
    rhs[i] = (double *)malloc(ny*sizeof(double));

  grid(nx,xst,xen,x,&dx);            // initialize the grid in x
  grid(ny,yst,yen,y,&dy);            // initialize the grid in y

  set_initial_condition(nx,ny,x,y,T,dx,dy);  // initial condition

  // prepare for time loop
  min_dx_dy = fmin(dx, dy);
  dt = 0.1 / kdiff * (min_dx_dy * min_dx_dy);   // Ensure r satisfies the stability condition
  num_time_steps = (int)((ten-tst)/dt) + 1;     // why add 1 to this?
  it_print = num_time_steps/10;                 // write out approximately 10 intermediate results
  printf("it_print=%d %d\n", it_print, num_time_steps);

  // start time stepping loop
  for(it=0; it<num_time_steps; it++)
  {
    tcurr = tst + (double)it * dt;

    timestep_FwdEuler(nx,ny,dt,dx,dy,kdiff,x,y,T,rhs);    // update T

    // output soln every it_print time steps
    if(it%it_print==0)
      output_soln(nx,ny,it,tcurr,x,y,T);
  }

  //for(i=0; i<nx; i++)
  //  free(T[i]);
  //free(T);
  //for(i=0; i<nx; i++)
  //  free(rhs[i]);
  //free(rhs);
  free(y);
  free(x);

  return 0;
}

