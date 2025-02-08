#include <stdio.h>
#include <stdlib.h>

#include "functions.h"

int main()
{

  int nx, nvars;
  double *x, *u, *p, *e, *rho;
  double **uvec, **flux, **umid, **fplus, **fminus;
  double tst, ten, xst, xen, dx, dt, tcurr, xpartition, CFL;
  double rho_left, rho_rght, p_left, p_rght, u_left, u_rght;
  int i, it, max_num_time_steps, it_print;
  FILE* fp;  

  // read inputs
  fp = fopen("input.in", "r");
  fscanf(fp, "%d\n", &nx);
  fscanf(fp, "%lf %lf %lf\n", &xst, &xen, &xpartition);
  fscanf(fp, "%lf %lf %lf\n", &tst, &ten, &CFL);
  fscanf(fp, "%lf %lf %lf\n", &rho_left, &u_left, &p_left);
  fscanf(fp, "%lf %lf %lf\n", &rho_rght, &u_rght, &p_rght);
  fclose(fp);

  printf("Inputs are: %d %lf %lf %lf %lf %lf %lf\n", nx, xst, xen, xpartition, tst, ten, CFL);
  printf("-L States-: %lf %lf %lf \n", rho_left, u_left, p_left);
  printf("-R States-: %lf %lf %lf \n", rho_rght, u_rght, p_rght);

  // allocate memory
  // all 1D arrays first

  x = (double *)malloc(nx*sizeof(double));
  u = (double *)malloc(nx*sizeof(double));
  p = (double *)malloc(nx*sizeof(double));
  e = (double *)malloc(nx*sizeof(double));
  rho = (double *)malloc(nx*sizeof(double));

  // now allocate 2D arrays
  // For 1D Euler Equations, size of vector of unknowns (U)
  // and vector of fluxes (F) is 3
  nvars = 3;

  // first allocate U
  uvec = (double **)malloc(nx*sizeof(double*));
  for(i=0; i<nx; i++)
    uvec[i] = (double *)malloc(nvars*sizeof(double));

  // now allocate F
  flux = (double **)malloc(nx*sizeof(double*));
  for(i=0; i<nx; i++)
    flux[i] = (double *)malloc(nvars*sizeof(double));
  
  // now allocate F(+)
  fplus = (double **)malloc(nx*sizeof(double*));
  for(i=0; i<nx; i++)
    fplus[i] = (double *)malloc(nvars*sizeof(double));

  // now allocate F(-)
  fminus = (double **)malloc(nx*sizeof(double*));
  for(i=0; i<nx; i++)
    fminus[i] = (double *)malloc(nvars*sizeof(double));

  // now allocate U intermediate
  umid = (double **)malloc(nx*sizeof(double*));
  for(i=0; i<nx; i++)
    umid[i] = (double *)malloc(nvars*sizeof(double));

  grid(nx,xst,xen,x,&dx);         // initialize the grid
  printf("Done grid setup\n");

  // initial conditions of u, p, rho
  set_initial_condition(nx, x, xpartition, rho, u, p,
         rho_left, u_left, p_left, rho_rght, u_rght, p_rght);
  printf("Done rho, u, p initial conditions\n");

  // initial conditions of e using eos
  eos_get_e_from_prho(nx, rho, p, e); 
  printf("Done initial conditions\n");

  // prepare for time loop
  it_print = 1;              // write out approximately 10 intermediate results
  max_num_time_steps = 10000;

  output_soln(nx, 0, tcurr, x, rho, u, p, e); // Write initial condition

  // start time stepping loop
  printf("Starting time loop\n");
  for(it=0; it<max_num_time_steps; it++)
  {
    if(tcurr > ten)
        break;

    get_dt(nx, CFL, rho, u, p, dx, &dt);
    tcurr = tcurr + dt;

    printf("---Time Step %d  %.6e---\n", it, dt);
    get_uvec_from_primitves(nx, x, rho, u, p, e, uvec);
    get_flux_from_primitves(nx, x, rho, u, p, e, flux);

    timestep_Lax(nx, nvars, dt, dx, uvec, flux, umid);
    //timestep_LaxWendroff(nx, nvars, dt, dx, x, rho, u, p, e, uvec, flux, umid);
    //timestep_vanLeer(nx, nvars, dt, dx, x, rho, u, p, e, uvec, flux, fplus, fminus);

    get_primitves_from_uvec(nx, x, rho, u, p, e, uvec);

    // output soln every it_print time steps
    if(it%it_print==0)
      output_soln(nx, it+1, tcurr, x, rho, u, p, e);
  }

  free(fplus);
  free(fminus);
  free(umid);
  free(flux);
  free(uvec);
  free(rho);
  free(e);
  free(p);
  free(u);
  free(x);

  return 0;
}

