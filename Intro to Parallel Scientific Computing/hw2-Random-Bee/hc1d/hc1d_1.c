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
  T[0] = 0.0;
  T[nx-1] = 0.0;
}

void set_initial_condition(int nx, double *x, double *T, double dx)
{
  int i;
  double del=1.0;

  for(i=0; i<nx; i++)
  {
    T[i] = 0.5 * (tanh((x[i]-0.45)/(del*dx)) - tanh((x[i]-0.65)/(del*dx)));
    //printf("\n%d %lf %lf %lf %lf", i, x[i], tanh((x[i]-0.45)/(del*dx)), tanh((x[i]-0.65)/(del*dx)), T[i]);
  }

  enforce_bcs(nx,x,T); //ensure BCs are satisfied at t = 0
}

void timestep_FwdEuler(int nx, double dt, double dx, double kdiff, double *x, double *T, double *rhs)
{

  int i;
  double dxsq = dx*dx;

  // (Forward) Euler scheme
  for(i=1; i<nx-1; i++)
    rhs[i] = (T[i+1]+T[i-1]-2.0*T[i]) * (kdiff/dxsq);  // compute rhs

  for(i=1; i<nx-1; i++)
    T[i] = T[i] + dt*rhs[i];                           // T^(it+1)[i] = T^(it)[i] + dt * rhs;

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
  double *x, *T, tst, ten, xst, xen, dx, dt, tcurr, kdiff;
  int i, it, num_time_steps, it_print;
  FILE* fp;  
  double *Tnew, *rhs;

  // read inputs
  fp = fopen("input.in", "r");
  fscanf(fp, "%d\n", &nx);
  fscanf(fp, "%lf %lf\n", &xst, &xen);
  fscanf(fp, "%lf %lf\n", &tst, &ten);
  fscanf(fp, "%lf\n", &kdiff);
  fclose(fp);

  printf("Inputs are: %d %lf %lf %lf %lf %lf\n", nx, xst, xen, tst, ten, kdiff);

  x = (double *)malloc(nx*sizeof(double));
  T = (double *)malloc(nx*sizeof(double));
  rhs = (double *)malloc(nx*sizeof(double));

  grid(nx,xst,xen,x,&dx);            // initialize the grid

  set_initial_condition(nx,x,T,dx);  // initial condition

  // prepare for time loop
  dt = 0.4 / kdiff * (dx*dx);               // Ensure r satisfies the stability condition
  num_time_steps = (int)((ten-tst)/dt) + 1; // why add 1 to this?
  it_print = num_time_steps/10;             // write out approximately 10 intermediate results
  printf("it_print=%d %d\n", it_print, num_time_steps);

  // start time stepping loop
  for(it=0; it<num_time_steps; it++)
  {
    tcurr = tst + (double)it * dt;

    timestep_FwdEuler(nx,dt,dx,kdiff,x,T,rhs);    // update T

    // output soln every it_print time steps
    if(it%it_print==0)
      output_soln(nx,it,tcurr,x,T);
  }

  free(rhs);
  free(T);
  free(x);

  return 0;
}

