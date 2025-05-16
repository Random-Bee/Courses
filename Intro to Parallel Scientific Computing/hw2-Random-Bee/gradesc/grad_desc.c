#include<stdio.h>
#include<stdlib.h>
#include<math.h>

void write_1darr_to_file(char *fname, double *arr, int n)
{
  FILE* fid;
  int i;
  fid = fopen(fname,"w");
  for(i=0; i<n; i++)
    fprintf(fid, "%.15e    ", arr[i]);
  fprintf(fid, "\n");
  fclose(fid);
}

void write_1darr_to_screen(double *arr, int n)
{
  int i;
  for(i=0; i<n; i++)
    printf("%.2e    ", arr[i]);
  printf("\n");
}

double calc_Jfun(double *y, double *yhat, int m)
{
  int i;
  double Jfun = 0.0;
  for(i=0; i<m; i++)
  {
    Jfun += (y[i] - yhat[i]) * (y[i] - yhat[i]);
    //printf("\n--In Jfun: %e %e %e", y[i], yhat[i], Jfun);
  }
  Jfun = 0.5*Jfun/(double) m;
  return Jfun;
}

void calc_yhat(double **x, double *theta, double *yhat, int m, int n)
{
  int i, j;
  for(i=0; i<m; i++)
  {
    yhat[i] = theta[0];
    for(j=1; j<n+1; j++)
      yhat[i] += theta[j]*x[i][j];
  }
  
  //printf("\n-- Printing array yhat:--\n");
  //write_1darr_to_screen(yhat, m);
  //printf("\n-- Done printing array yhat\n");

}

void calc_new_theta(double **x, double *theta, double *yhat, double *y, double learning_rate, int m, int n)
{
  int i, j;
  double gradJ; 

  for(j=0; j<n+1; j++)
  {
    gradJ = 0.0;
    for(i=0; i<m; i++)
      gradJ += (yhat[i]-y[i]) * x[i][j];
    theta[j] = theta[j] - learning_rate * gradJ;
  }
  
  //printf("\n-- Printing array theta:--\n");
  //write_1darr_to_screen(theta, n+1);
  //printf("\n-- Done printing array theta\n");

}
  
void write_data_to_file(char *fname, double **x, double *y, double *yhat, double *theta, int m, int n)
{

  FILE* fid;
  int i, j;

  //fid = fopen("Student_Performance.csv","r");
  fid = fopen(fname,"w");
  for(i=0; i<m; i++)
  {
    for(j=0; j<n+1; j++)
      fprintf(fid, "%.6e ", x[i][j]);
    fprintf(fid, "%.6e %.6e\n", y[i], yhat[i]);
  }

}
  
void read_data_from_file(char *fname, double **x, double *y, int m, int n)
{

  FILE* fid;
  int i, j;

  //fid = fopen("Student_Performance.csv","r");
  fid = fopen(fname,"r");
  for(i=0; i<m; i++)
  {
    //fscanf(fid,"%lf,%lf,%lf,%lf,%lf\n", &x[i][0], &x[i][1], &x[i][2], &x[i][3], &y[i]) ;
    x[i][0] = 1.0; // dummy feature
    for(j=1; j<n+1; j++)
      fscanf(fid,"%lf,", &x[i][j]) ;
    fscanf(fid,"%lf\n", &y[i]) ;
  }
  fclose(fid);

  //// check if data was read in correctly
  //for(i=0;i<m;i++)
  //{
  //  printf("%lf, %lf, %lf, %lf, %lf, %lf\n", x[i][0], x[i][1], x[i][2], x[i][3], x[i][4], y[i]);
  //}
} 
  
void scale_and_translate_data(double **x, double *y, double *means, double* sdevs, int m, int n)
{
  int i, j;

  // calculate the mean and the sdev of each feature/output
  for(j=0; j<n+2; j++)
  {
    means[j] = 0.0;
    sdevs[j] = 0.0;
  }
  for(i=0; i<m; i++)
  {
    for(j=0; j<n+1; j++)
    {
      means[j] += x[i][j];
      sdevs[j] += x[i][j]*x[i][j];
    }
    means[n+1] += y[i];
    sdevs[n+1] += y[i]*y[i];
  }
  for(j=0; j<n+2; j++)
  {
    means[j] = means[j]/((double) m);
    sdevs[j] = sqrt(sdevs[j]/((double) m) - means[j]*means[j]);
    //printf("mean, sdev of %d is %e, %e\n", j, means[j], sdevs[j]);
  }

  // now actually translate and scale the data
  for(i=0; i<m; i++)
  {
    // dummy feature has sdev 0; only translate it
    j = 0;   x[i][j] = ( x[i][j] - means[j] );

    // translate and scale the rest of the features
    for(j=1; j<n+1; j++)
      x[i][j] = ( x[i][j] - means[j] ) / sdevs[j];

    // translate and scale the labels
    y[i] = ( y[i] - means[n+1] ) / sdevs[n+1];
  }
  
}

void undo_scale_and_translate_data(double **x, double *y, double *yhat, double *means, double* sdevs, int m, int n)
{
  int i, j;

  // redo translate and scale using the same means and sdevs computed earlier
  for(i=0; i<m; i++)
  {
    // dummy feature has sdev 0; only translate it
    j = 0;  x[i][j] = x[i][j] + means[j];

    // translate and scale the rest of the features
    for(j=1; j<n+1; j++)
      x[i][j] = x[i][j] * sdevs[j] + means[j];

    // translate and scale the labels and the prediction
    y[i]    = y[i]    * sdevs[n+1] + means[n+1];
    yhat[i] = yhat[i] * sdevs[n+1] + means[n+1];
  }  
  
}


int main()
{ 

  int m, n, i, j, k, max_iter;
  double **x, *y, *yhat, *theta, tol, learning_rate;
  double *means, *sdevs, Jfun0, Jfun, Jfun_old;
  FILE* fid;

  // read data
  m = 10000;   // size of data
  n = 4;       // number of features
  
  // allocate 2d array for feature vector
  // of size m x (n+1) :: dummy feature included
  x = (double**) malloc(m*sizeof(double*));
  for(i=0; i<m; i++)
    x[i] = (double*) malloc((n+1)*sizeof(double));

  // allocate 1d array for labels
  y = (double*) malloc(m*sizeof(double));

  // allocate 1d array for means and sdevs of features and label
  means = (double*) malloc((n+2)*sizeof(double));
  sdevs = (double*) malloc((n+2)*sizeof(double));
 
  // read in the data
  read_data_from_file("Student_Performance.csv", x, y, m, n);
  scale_and_translate_data(x, y, means, sdevs, m, n);
  //for(i=0;i<m;i++)
  //  printf("%lf, %lf, %lf, %lf, %lf, %lf\n", x[i][0], x[i][1], x[i][2], x[i][3], x[i][4], y[i]);

  // allocate 1d array for parameters
  theta = (double*) malloc((n+1)*sizeof(double));

  // initialize parameters
  for(j=0; j<n+1; j++)
    theta[j] = 0.0;

  // allocate 1d array for predictions
  yhat = (double*) malloc(m*sizeof(double));

  // specify iteration loop parameters
  max_iter = 400;  tol = 1.0e-6;   learning_rate = 0.2/(double) m;

  // calcualte initial value of J function
  calc_yhat(x, theta, yhat, m, n);
  Jfun_old = calc_Jfun(y, yhat, m);
  Jfun0 = Jfun_old; // used as a reference scale
  printf("\nIter: %03d: Jfun = %e", -1, Jfun_old);

  // start iterations
  for(k=0; k<max_iter; k++)
  {
    calc_new_theta(x, theta, yhat, y, learning_rate, m, n);
    calc_yhat(x, theta, yhat, m, n);
    Jfun = calc_Jfun(y, yhat, m);
    printf("\nIter: %03d: Jfun = %0.12e", k, Jfun);
    if( (fabs(Jfun-Jfun_old) < tol) || (fabs(Jfun) < tol*Jfun0))
      break;
    else
    {
      Jfun_old = Jfun;
    }
  }
  
  write_1darr_to_file("fit_parameters.dat", theta, n+1);
  write_data_to_file("fitting_results_scaled.dat", x, y, yhat, theta, m, n);

  undo_scale_and_translate_data(x, y, yhat, means, sdevs, m, n);
  //for(i=0;i<m;i++)
  //  printf("%lf, %lf, %lf, %lf, %lf, %lf\n", x[i][0], x[i][1], x[i][2], x[i][3], x[i][4], y[i]);
  write_data_to_file("fitting_results_unscaled.dat", x, y, yhat, theta, m, n);
 
 
  // free allocated memory 
  for(i=0; i<m; i++)
    free(x[i]);
  free(x);
  free(y);
  free(yhat);
  free(theta);
  free(means);
  free(sdevs);

  printf("\n");
  return 0;
}
