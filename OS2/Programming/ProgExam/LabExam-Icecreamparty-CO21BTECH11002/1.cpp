#include <bits/stdc++.h>
#include <semaphore.h>
using namespace std;

sem_t ven_mut, fam_mut, ice_mut;
// currM = no. of icecreams currently in tray
// tot = total no. of icreams eaten by now
int k,m,n,currM,tot=0;
double al,be,la,mu;
double avgEat=0, avgFill=0;
sem_t avgEatMut, totMut;
// no. of times vendor fills the pot
int noOfFills=0;

default_random_engine random_number_generator;

// Pointer to log file
FILE* f_out;

time_t init,curr;

void ven_func() {
    int iceFilled=0;
    exponential_distribution<double> exp1(1.0/la);
    exponential_distribution<double> exp2(1.0/mu);
    while(true) {
        if(tot >= n*k) {
            return;
        }
        sem_wait(&ven_mut);
        if(tot >= n*k) {
            return;
        }

        // fill ice cream
        time_t fillTime = time(NULL);
        noOfFills++;

        fprintf(f_out,"Time%d: Vendor refills the ice cream %d times\n",fillTime-init, fillTime-init);

        double t = exp1(random_number_generator)*1000000; // time in microseconds
        usleep(t);
        iceFilled += m;
        currM += m;

        time_t doneFilling = time(NULL);

        avgFill += doneFilling-fillTime;

        sem_post(&fam_mut);

        // sleep
        t = exp2(random_number_generator)*1000000; // time in microseconds
        usleep(t);
    }
}

void fam_func(int fam_id) {
    exponential_distribution<double> exp1(1.0/al);
    exponential_distribution<double> exp2(1.0/be);
    double t;
    for(int i=0; i<n; i++) {

        // this fam got hungry
        time_t StartTime = time(NULL);

        fprintf(f_out,"Time%d: Family %d becomes hungry\n",StartTime-init, fam_id);

        // check curr no. of available ice creams
        sem_wait(&ice_mut);
        if(currM==0) {
            sem_post(&ven_mut);
            sem_wait(&fam_mut);
        }
        // got the ice cream, yayyy!
        currM--;
        sem_post(&ice_mut);

        // eat ice cream!
        time_t eatNow = time(NULL);

        sem_wait(&avgEatMut);
        avgEat += eatNow - StartTime;
        sem_post(&avgEatMut);

        fprintf(f_out,"Time%d: Family %d eats from the Pot\n",eatNow-init, fam_id);
        t = exp1(random_number_generator)*1000000; // time in microseconds
        usleep(t);

        sem_wait(&totMut);
        tot++;
        sem_post(&totMut);
        
        // community service
        t = exp2(random_number_generator)*1000000; // time in microseconds
        usleep(t);
    }

    time_t exitNow = time(NULL);
    fprintf(f_out,"Time%d: Family %d has eaten %d times. Hence, exits.\n",exitNow-init, fam_id, n);

    // every one is done eating
    if(tot==n*k) {
        time_t finExit = time(NULL);
        fprintf(f_out,"Time %d: Last thread exits\n",finExit-init);
        sem_post(&ven_mut);
    }

}

int main(int argc, char* argv[]) {

    // Read input from file
    FILE *f1;
    f1 = fopen(argv[1], "r");
    fscanf(f1,"K = %d\nM = %d\nn = %d\nalpha = %lf\nbeta = %lf\nlambda = %lf\nmu = %lf",&k,&m,&n,&al,&be,&la,&mu);
    fclose(f1);

    currM = m;

    sem_init(&totMut,0,1);
    sem_init(&avgEatMut,0,1);
    sem_init(&ice_mut,0,1);
    sem_init(&ven_mut,0,0);
    sem_init(&fam_mut,0,0);

    // Open output file
    f_out = fopen("Out.txt","w");

    init = time(NULL);
    curr = init;
    fprintf(f_out,"Time%d : K threads are created\n",curr-init);

    // create vender thread
    vector<thread> ven;
    ven.push_back(thread(ven_func));

    vector<thread> fams;
    for(int i=0; i<k; i++) {
        // create families
        fams.push_back(thread(fam_func,i+1));
    }

    ven[0].join();
    for(int i=0; i<k; i++) {
        fams[i].join();
    }

    fclose(f_out);

    avgEat = (avgEat)/(1.0*n*k);
    avgFill = (avgFill)/(1.0*noOfFills);

    cout << "Average waiting time to eat: " << avgEat << endl;
    cout << "Average waiting time to fill: " << avgFill << endl;

    return 0;
}