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

void eos_get_e_from_prho(int nx, double *rho, double *p, double *e)
{
  int i;
  double gam_m1 = 0.4;  // gamma is 1.4 for air

  for(i=0; i<nx; i++)
    e[i] = p[i]/(gam_m1 * rho[i]);
}

void eos_get_p_from_erho(int nx, double *rho, double *e, double *p)
{
  int i;
  double gam_m1 = 0.4;  // gamma is 1.4 for air

  for(i=0; i<nx; i++)
    p[i] = e[i] * gam_m1 * rho[i];
}

void set_initial_condition(int nx, double *x, double xpartition, 
                           double *rho,     double *u,     double *p,
                           double rho_left, double u_left, double p_left,
                           double rho_rght, double u_rght, double p_rght)
{
  int i, index_partition;

  // determine index corresponding to xpartition
  for(i=0; i<nx; i++)
    if(x[i]>xpartition)
      break;
  index_partition = i-1;
  if(index_partition < 0)
  {
    printf("xpartition is not set correctly. Check details.\n");
    exit(0);
  }

  // set rho
  for(i=0; i<=index_partition; i++)
    rho[i] = rho_left;

  for(i=index_partition+1; i<nx; i++)
    rho[i] = rho_rght;

  // set u
  for(i=0; i<=index_partition; i++)
    u[i]   = u_left;

  for(i=index_partition+1; i<nx; i++)
    u[i] = u_rght;

  // set p
  for(i=0; i<=index_partition; i++)
    p[i]   = p_left;

  for(i=index_partition+1; i<nx; i++)
    p[i] = p_rght;
}

void get_flux_from_primitves(int nx, double *x, double *rho, 
           double *u, double *p, double *e, double **flux)
{
  int i;

  // Compute F from primitive vars
  // F(1) :: rho*u
  // F(2) :: rho*u^2 + p
  // F(3) :: rho*(e + u^2/2)*u + p*u
  for(i=0; i<nx; i++)
  {
    flux[i][0] = rho[i]*u[i];
    flux[i][1] = rho[i]*u[i]*u[i] + p[i];
    flux[i][2] = rho[i]*(e[i] + 0.5*u[i]*u[i])*u[i] + p[i]*u[i];
  }
}

void get_uvec_from_primitves(int nx, double *x, double *rho, 
           double *u, double *p, double *e, double **uvec)
{
  int i;

  // Pack primitive vars into U
  // U(1) :: rho
  // U(2) :: rho*u
  // U(3) :: rho*(e + u^2/2)
  //printf("--In get_uvec_from_primitives--\n");
  for(i=0; i<nx; i++)
  {
    //printf("    ** %d %.6e\n", i, rho[i]);
    uvec[i][0] = rho[i];
    uvec[i][1] = rho[i] * u[i];
    uvec[i][2] = rho[i] * (e[i] + 0.5*u[i]*u[i]);
  }
  //printf("--Done get_uvec_from_primitives--\n");
}

void get_dt(int nx, double CFL, double *rho, double *u, double *p, double dx, double *dt)
{
  int i;
  double umax, uloc, gam;

  gam = 1.4;  umax = 0.0;
  for(i=0; i<nx; i++)
  {
    uloc = sqrt(gam*p[i]/rho[i]) + fabs(u[i]);
    if(umax < uloc)
      umax = uloc;
  }

  *dt = CFL*dx/umax;

}

void get_primitves_from_uvec(int nx, double *x, double *rho, 
           double *u, double *p, double *e, double **uvec)
{
  int i;

  // Unpack components of U to get primitive vars
  for(i=0; i<nx; i++)
  {
    rho[i] = uvec[i][0];
    u[i]   = uvec[i][1]/rho[i];
    e[i]   = uvec[i][2]/rho[i] - 0.5*u[i]*u[i];
  }

  eos_get_p_from_erho(nx, rho, e, p);
}

void timestep_Lax(int nx, int nvars, double dt, double dx, double **uvec, double **flux, double **umid)
{
  int i, j;

  double dtdxfac;

  dtdxfac = 0.5*dt/dx;

  for(i=1; i<nx-1; i++)
    for(j=0; j<nvars; j++)
    {
      umid[i][j] = -dtdxfac*(flux[i+1][j] - flux[i-1][j]);
      umid[i][j] += 0.5*(uvec[i+1][j] + uvec[i-1][j]);
    }

  for(i=1; i<nx-1; i++)
    for(j=0; j<nvars; j++)
      uvec[i][j] = umid[i][j];
}

void timestep_LaxWendroff(int nx, int nvars, double dt, double dx, double *x, 
                 double *rho, double *u, double *p, double *e, double **uvec, double **flux, double **umid)
{
  int i, j;
  double dtdxfac;
  FILE* fp;

  // predictor step -- compute umid from uvec and flux
  dtdxfac = 0.5*dt/dx;


  // corrector step
  // -- step 1: compute primitives corresponding to umid
  // -- step 2: compute flux corresponding to umid
  // -- step 3: then update uvec
  dtdxfac = dt/dx;

}

void timestep_vanLeer(int nx, int nvars, double dt, double dx, double *x, 
                double *rho, double *u, double *p, double *e, double **uvec, 
                double **flux, double **fplus, double **fminus)
{
  int i, j;
  double dtdxfac, fac, fac2, sos, Ma, gam = 1.4;
  FILE* fp;

  // get F(+) at i
  

  // get F(-) at i from F and F(+) at i 
  for(i=0; i<nx; i++)
    for(j=0; j<nvars; j++)
        fminus[i][j] = flux[i][j] - fplus[i][j];


  // get flux at (i+1/2) from F(+) and F(-) at i

  // update solution vector
  dtdxfac = dt/dx;

}

void output_soln(int nx, int it, double tcurr, double *x, double *rho, double *u, double *p, double *e)
{
  int i;
  FILE* fp;
  char fname[100];

  sprintf(fname, "primitives_x_%04d.dat", it);
  //printf("\n%s\n", fname);

  fp = fopen(fname, "w");
  for(i=0; i<nx; i++)
    fprintf(fp, "%.6e %.6e %.6e %.6e %.6e\n", x[i], rho[i], u[i], p[i], e[i]);
  fclose(fp);

  printf("Done writing solution for time step = %d\n", it);
}
