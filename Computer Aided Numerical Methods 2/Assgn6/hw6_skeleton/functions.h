void grid(int nx, double xst, double xen, double *x, double *dx);
void eos_get_e_from_prho(int nx, double *rho, double *p, double *e);
void eos_get_p_from_erho(int nx, double *rho, double *e, double *p);
void set_initial_condition(int nx, double *x, double xpartition, 
                           double *rho,     double *u,     double *p,
                           double rho_left, double u_left, double p_left,
                           double rho_rght, double u_rght, double p_rght);
void get_flux_from_primitves(int nx, double *x, double *rho, 
           double *u, double *p, double *e, double **flux);
void get_uvec_from_primitves(int nx, double *x, double *rho, 
           double *u, double *p, double *e, double **uvec);
void get_primitves_from_uvec(int nx, double *x, double *rho, 
           double *u, double *p, double *e, double **uvec);
void timestep_Lax(int nx, int nvars, double dt, double dx, double **uvec, double **flux, double **umid);
void timestep_LaxWendroff(int nx, int nvars, double dt, double dx, double *x, double *rho, double *u, double *p, double *e, double **uvec, double **flux, double **umid);
void  timestep_MacCormack(int nx, int nvars, double dt, double dx, double *x, double *rho, double *u, double *p, double *e, double **uvec, double **flux, double **umid);
void timestep_vanLeer(int nx, int nvars, double dt, double dx, double *x, 
                double *rho, double *u, double *p, double *e, double **uvec, 
                double **flux, double **fplus, double **fminus);
void output_soln(int nx, int it, double tcurr, double *x, double *rho, double *u, double *p, double *e);
void get_dt(int nx, double CFL, double *rho, double *u, double *p, double dx, double *dt);
