#include <stdio.h>
#include <stdlib.h>

#define row(n,i,j) ((n-2)*(n-2)*((n-2)*(i-1)+(j-1)))
#define col(n,i,j) ((n-2)*(i-1)+(j-1))

double gd(double a,double i,double d){
	return (2*i*d-a);
}
double g(double a,double i,double j, double dx,double dy){
	return ((i*dx-a/2)*(i*dx-a/2)+(j*dy-a/2)*(j*dy-a/2));
}

void print(int n,double *arr){
    
    for (int i=0;i<n+2;i++)  printf("%d  %d  %d\n",0,i,150);
	for (int i=0;i<n;i++){
		for (int j=0;j<n;j++){
            if (j==0) printf("%d  %d  %d\n",i+1,j,50);
			printf("%d  %d  %lf\n",i+1,j+1,arr[n*i+j]);
            if (j==n-1) printf("%d  %d  %d\n",i+1,n+1,50);
		}
	}for (int i=0;i<n+2;i++)  printf("%d  %d  %d\n",n+1,i,50);
}

void mtxA(int n,double *arr,double a,double *b,double p1,double p3){
	double dx = a/(n-1), dy = a/(n-1);
	for (int i=1;i<=n-2;i++){
		for (int j=1;j<=n-2;j++){
			arr[row(n,i,j)+col(n,i,j)] = -2*(g(a,i,j,dx,dy)/(dx*dx) + g(a,i,j,dx,dy)/(dy*dy));
			if (i<n-2) arr[row(n,i,j)+col(n,i+1,j)] =  g(a,i,j,dx,dy)/(dx*dx)+gd(a,i,dx)/(2*dx);
			if (i>1)   arr[row(n,i,j)+col(n,i-1,j)] =  g(a,i,j,dx,dy)/(dx*dx)-gd(a,i,dx)/(2*dx);
			if (j<n-2) arr[row(n,i,j)+col(n,i,j+1)] =  g(a,i,j,dx,dy)/(dy*dy)+gd(a,j,dy)/(2*dy);
			if (j>1)   arr[row(n,i,j)+col(n,i,j-1)] =  g(a,i,j,dx,dy)/(dy*dy)-gd(a,j,dy)/(2*dy);
            if (i==1){
                if (j==1)        b[col(n,i,j)] = -p1*(g(a,i,j,dx,dy)/(dx*dx)-gd(a,i,dx)/(2*dx)) -p3*(g(a,i,j,dx,dy)/(dx*dx)-gd(a,j,dy)/(2*dy));
                else if (j==n-2) b[col(n,i,j)] = -p1*(g(a,i,j,dx,dy)/(dx*dx)-gd(a,i,dx)/(2*dx)) -p3*(g(a,i,j,dx,dy)/(dy*dy)+gd(a,j,dy)/(2*dy));
                else             b[col(n,i,j)] = -p1*(g(a,i,j,dx,dy)/(dx*dx)-gd(a,i,dx)/(2*dx));
            }else if (i==n-2){
                if (j==1)        b[col(n,i,j)] = -p3*(g(a,i,j,dx,dy)/(dy*dy)-gd(a,j,dy)/(2*dy)) -p3*(g(a,i,j,dx,dy)/(dx*dx)+gd(a,i,dx)/(2*dx));
                else if (j==n-2) b[col(n,i,j)] = -p3*(g(a,i,j,dx,dy)/(dx*dx)+gd(a,i,dx)/(2*dx)) -p3*(g(a,i,j,dx,dy)/(dy*dy)+gd(a,j,dy)/(2*dy));
                else             b[col(n,i,j)] = -p3*(g(a,i,j,dx,dy)/(dx*dx)+gd(a,i,dx)/(2*dx));
            }else {
                if (j==1)        b[col(n,i,j)] = -p3*(g(a,i,j,dx,dy)/(dy*dy)-gd(a,j,dy)/(2*dy));
                else if (j==n-2) b[col(n,i,j)] = -p3*(g(a,i,j,dx,dy)/(dy*dy)+gd(a,j,dy)/(2*dy));
            }
		}
	}return;
}

int err(int n,double *arr,double *x,double *x0,double *tol,double *w){
    double error=0;
    double r=0;
    for (int i=0;i<n;i++) x0[i] -= x[i]; 
    for (int i =0;i<n;i++){
        for (int j=0;j<n;j++) r += arr[n*i+j]*(x0[j]);
        error+=r*r;
    }printf("%.11f\n",error);
    if (*tol-error>0) *w = 1.9;
    else *w = 0.9;
    *tol = error;
    if (error<1e-10) return 0;
    else return 1;
}

void GS(double *arr,int n,double *b,double *x){
    for (int i=0;i<n;i++) x[i] = 50;
    double *x0 = calloc(n,sizeof(double));
    int it=0;
    double sum,tol;
    double w=1;
    while (err(n,arr,x,x0,&tol,&w)){
        it++;
        for (int i=0;i<n;i++) x0[i]= x[i];
        for (int i=0;i<n;i++) {
            sum = b[i];
            for (int j=0;j<n;j++){
				if (i!=j) sum-=arr[n*i+j]*x[j];
            }x[i]= (1-w)*x0[i]+w*sum/arr[n*i+i];  
        }
    }printf("%d\n",it);
    free(x0);
    return;
}

int main(){
    double n=100,a=99;
    double n1 = (n-2)*(n-2);
	double *arr = calloc(n1*n1,sizeof(double));
    double *b = calloc(n1,sizeof(double));
    double *x=calloc(n1,sizeof(double));
    mtxA(n,arr,a,b,150,50);
    //for (int i=0;i<(n-2)*(n-2);i++) printf("%lf\n",b[i]);
    //print((n-2),x);
    GS(arr,n1,b,x);
    print(n-2,x);
    free(arr);
    free(b);
    free(x);
    return 0;
}