#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
int isPerfect(int n) {
    if(n==1) return 0;
    int i,j,sum=1;
    for(i=2; i*i<=n; i++) {
        if(n%i==0) {
            if(i*i != n) sum += i + n/i;
            else sum += i;
        }
    }
    if(sum==n) return 1;
    else return 0;
}
int min(int a, int b) {
    return a<b? a: b;
}
int main(int argc, char* argv[]) { 
    FILE* fp;
    FILE* fd;
    FILE* fin;
    char buffer[50];
    int n,k,ct,localArr[100];

    // open the file corresponding to main process
    fd = fopen("OutMain.txt", "w");
    
    fin = fopen(argv[1], "r");
    if(fin != NULL) {
        // read the input numbers
        fscanf(fin, "%d", &n);
        fscanf(fin, "%d", &k);
        
        // conut of numbers that each process will get to check
        ct = n/k;
        if(n%k!=0) ct++;
        
        for(int i=0; i<k; i++) {
            pid_t c = fork();
            if(c == 0) {
                // if child, enter.
                int cn=0; // count of perect numbers

                sprintf(buffer, "OutFile%d.txt\n", i); // buffer would store the name of file to be created by the correseponding child process
                fp = fopen(buffer,"w");
                
                for(int j=i*ct+1; j<=min((i+1)*ct,n); j++) {
                    if(isPerfect(j)==1) {
                        localArr[cn] = j;
                        fprintf(fp, "%d is a perfect number\n", j);
                        cn++;
                    }
                    else {
                        fprintf(fp, "%d is not a perfect number\n", j);
                    }
                }

                fclose(fp);
                
                // if no. of perfect numbers is not 0 the modify the main file
                if(cn!=0) {
                    char s[100]; // temporary string to store the process name and the numbers found
                    sprintf(s, "P%d - ", i);
                    for(int j=0; j<cn; j++) {
                        sprintf(buffer, "%d ", localArr[j]);
                        strcat(s,buffer);
                    }
                    strcat(s,"\n");
                    fputs(s,fd);
                }

                // exiting would make sure the the child process do not create another child process of its own
                exit(0);
            }
        }
        fclose(fin);
    }
    fclose(fd);
}