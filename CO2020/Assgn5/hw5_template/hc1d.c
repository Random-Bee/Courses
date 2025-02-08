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

void enforce_bcs(int nx, double *x, double *T)
{
  T[0] = -1.0;
  T[nx-1] = 1.0;
}

void set_initial_condition(int nx, double *x, double *T)
{
  int i;

  for(i=0; i<nx; i++)
    T[i] = tanh((x[i]-0.5)/0.05);

  enforce_bcs(nx,x,T); //ensure BCs are satisfied at t = 0
}

void get_rhs(int nx, double dx, double *x, double *T, double *rhs)
{
  int i;
  double dxsq = dx*dx;

  // compute rhs. For this problem, d2T/dx2
  for(i=1; i<nx-1; i++)
    rhs[i] = (T[i+1]+T[i-1]-2.0*T[i])/dxsq;
}

void timestep_Euler(int nx, double dt, double dx, double *x, double *T, double *rhs)
{

  int i;

  // compute rhs
  get_rhs(nx,dx,x,T,rhs);

  // (Forward) Euler scheme
  for(i=0; i<nx; i++)
    T[i] = T[i] + dt*rhs[i];  // T^(it+1)[i] = T^(it)[i] + dt * rhs[i];

  // set Dirichlet BCs
  enforce_bcs(nx,x,T);

}

void output_soln(int nx, int it, double tcurr, double *x, double *T)
{
  int i;
  FILE* fp;
  char fname[100];

  sprintf(fname, "T_x_%04d.dat", it);
  //printf("\n%s\n", fname);

  fp = fopen(fname, "w");
  for(i=0; i<nx; i++)
    fprintf(fp, "%lf %lf\n", x[i], T[i]);
  fclose(fp);

  printf("Done writing solution for time step = %d\n", it);
}

int main()
{

  int nx;
  double *x, *T, *rhs, tst, ten, xst, xen, dx, dt, tcurr;
  int i, it, num_time_steps, it_print;
  FILE* fp;  

  // read inputs
  fp = fopen("input.in", "r");
  fscanf(fp, "%d\n", &nx);
  fscanf(fp, "%lf %lf\n", &xst, &xen);
  fscanf(fp, "%lf %lf\n", &tst, &ten);
  fclose(fp);

  printf("Inputs are: %d %lf %lf %lf %lf\n", nx, xst, xen, tst, ten);

  x = (double *)malloc(nx*sizeof(double));
  T = (double *)malloc(nx*sizeof(double));
  rhs = (double *)malloc(nx*sizeof(double));
  

  grid(nx,xst,xen,x,&dx);         // initialize the grid

  set_initial_condition(nx,x,T);  // initial condition

  // prepare for time loop
  dt = 0.0001;                               // Ensure r satisfies the stability condition
  num_time_steps = (int)((ten-tst)/dt) + 1; // why add 1 to this?
  it_print = num_time_steps/10;             // write out approximately 10 intermediate results

  // start time stepping loop
  for(it=0; it<num_time_steps; it++)
  {
    tcurr = tst + (double)it * dt;

    timestep_Euler(nx,dt,dx,x,T,rhs);    // update T

    // output soln every it_print time steps
    if(it%it_print==0)
      output_soln(nx,it,tcurr,x,T);
  }

  free(rhs);
  free(T);
  free(x);

  return 0;
}

