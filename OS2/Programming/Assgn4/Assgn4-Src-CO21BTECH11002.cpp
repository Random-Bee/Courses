#include <bits/stdc++.h>
#include <semaphore.h>
using namespace std;

// Total time taken by passengers to enter and exit the museum, total riding time for cars
double pas_total=0, car_total=0;
// mutexes for updating pas_total and car_total
sem_t pas_mutex, car_mutex;

// Pointer to log file
FILE* f_out;

// no. of cars, no. of passengers, no. of requests made by each passenger
int n,p,k;
// exponential wait time parameters for cars and passengers
double lam_p,lam_c;

// total no. of requests completed
int total=0;
// mutex for updating total
sem_t tot_mutex;

// pas_ids - stores id of passenger travelling in i'th car
// avl_cars - works as a queue, storing ids of cars available
vector<int> pas_ids, avl_cars;
// avl_cars_mutex - mutex for updating avl_cars queue
sem_t avl_cars_mutex;

// pas-state - stores state of each passenger (riding if 0, waiting if 1), car_state - stores state of each car (riding if 0, waiting if 1)
vector<sem_t> pas_state, car_state;
// empty_mutex - no. of cars waiting
sem_t empty_mutex;

// random number generator for exponential wait time
default_random_engine random_number_generator;

// Funtion for car thread
void car_func(int car_id) {
    while(true) {

        // wait for a passenger to request a ride
        sem_wait(&car_state[car_id]);

        // if total no. of requests completed is p*k, return
        if(total == p*k) {
            return;
        }

        // start the ride
        time_t StartTime = time(NULL);

        // print ride start time in log file
        fprintf(f_out,"Car %d is riding passenger %d\n", car_id, pas_ids[car_id]);

        // ride for random time
        exponential_distribution<double> exp1(1.0/lam_c);

        double t = exp1(random_number_generator)*1000000; // time in microseconds
        usleep(t);

        // end the ride
        // print ride end time in log file
        fprintf(f_out,"Car %d has finished passenger %d's tour\n", car_id, pas_ids[car_id]);

        // signal passenger that ride is over
        sem_post(&pas_state[pas_ids[car_id]]);

        // update car_total variable
        time_t EndTime = time(NULL);

        sem_wait(&car_mutex);
        car_total += EndTime - StartTime;
        sem_post(&car_mutex);

        // wait for some random time before taking another passenger
        t = exp1(random_number_generator)*1000000; // time in microseconds
        usleep(t);

        // add car to queue of available cars
        sem_wait(&avl_cars_mutex);
        avl_cars.push_back(car_id);
        // increment no. of cars waiting
        sem_post(&empty_mutex);
        sem_post(&avl_cars_mutex);


    }
}

void pas_func(int pas_id) {
    
    // passenger entered the museum
    time_t EnterTime = time(NULL);
    tm* enter_now = localtime(&EnterTime);
    
    // print entry time in log file
    fprintf(f_out,"Passenge %d enters the museum at %d:%d:%d\n", pas_id, enter_now->tm_hour, enter_now->tm_min, enter_now->tm_sec);

    // wait for some random time before requesting a ride
    exponential_distribution<double> exp2(1.0/lam_p);

    double t = exp2(random_number_generator)*1000000; // time in microseconds
    usleep(t);
    
    for(int i=0; i<k; i++) {

        // requested a ride
        time_t reqEnterTime = time(NULL);
        tm* req_now = localtime(&reqEnterTime);

        // print request time in log file
        fprintf(f_out,"Passenge %d requests a ride at %d:%d:%d\n", pas_id, req_now->tm_hour, req_now->tm_min, req_now->tm_sec);

        // wait for a car to be available
        sem_wait(&empty_mutex);

        // get the first available car
        sem_wait(&avl_cars_mutex);
        int car_id = avl_cars.front();

        // print car accept request time in log file
        fprintf(f_out,"Car %d accepts passenger %d's request\n", car_id, pas_id);

        // remove car from queue of available cars
        avl_cars.erase(avl_cars.begin());

        // signal avl_cars_mutex so that other threads could modify avl_cars queue
        sem_post(&avl_cars_mutex);

        // store id of passenger travelling in car car_id
        pas_ids[car_id] = pas_id;

        // starts the ride
        time_t StartTime = time(NULL);
        tm* start_now = localtime(&StartTime);

        // print ride start time in log file
        fprintf(f_out,"Passenger %d started riding %d:%d:%d\n", pas_id, start_now->tm_hour, start_now->tm_min, start_now->tm_sec);

        // signal car to start the ride
        sem_post(&car_state[car_id]);

        // wait for ride to end
        sem_wait(&pas_state[pas_id]);

        // end the ride
        time_t EndTime = time(NULL);
        tm* end_now = localtime(&EndTime);

        // print ride end time in log file
        fprintf(f_out,"Passenger %d finished riding at %d:%d:%d\n", pas_id, end_now->tm_hour, end_now->tm_min, end_now->tm_sec);

        // update total no. of requests completed
        sem_wait(&tot_mutex);
        total++;
        sem_post(&tot_mutex);

        // wait for some random time before requesting another ride
        t = exp2(random_number_generator)*1000000; // time in microseconds
        usleep(t);
    }

    // passenger left the museum
    time_t ExitTime = time(NULL);
    tm* exit_now = localtime(&ExitTime);

    // print exit time in log file
    fprintf(f_out,"Passenger %d exit the museum at %d:%d:%d\n", pas_id, exit_now->tm_hour, exit_now->tm_min, exit_now->tm_sec);

    // update pas_total
    sem_wait(&pas_mutex);
    pas_total += ExitTime - EnterTime;
    sem_post(&pas_mutex);

    // check if all requests have been completed
    // once all the rides are over, the car_state can be changed by any thread in any order as that does not affect the output
    // hence, the update does not need to be mutually exclusive
    if(total == p*k) {
        for(int i=0; i<n; i++) {
            sem_post(&car_state[i+1]);
        }
    }
}

int main(int argc, char* argv[]) {
    
    // Read input from file
    FILE *f1;
    f1 = fopen(argv[1], "r");
    fscanf(f1,"%d %d %lf %lf %d",&p,&n,&lam_p,&lam_c,&k);
    fclose(f1);

    // Initialize all the required vectors, 1-indexed
    pas_ids.resize(n+1,0);
    pas_state.resize(p+1);
    car_state.resize(n+1);

    // Initialize semaphores, initially all are 1
    sem_init(&car_mutex,0,1);
    sem_init(&pas_mutex,0,1);
    sem_init(&tot_mutex,0,1);
    sem_init(&avl_cars_mutex,0,1);

    // initially all cars are available, hence initially empty_mutex = n
    sem_init(&empty_mutex,0,n);
    
    // initialize car_state and pas_state semaphores
    // initially all cars and passengers are waiting
    for(int i=0; i<n+1; i++) {
        sem_init(&car_state[i],0,0);
    }
    for(int i=0; i<p+1; i++) {
        sem_init(&pas_state[i],0,0);
    }

    // Open output file
    f_out = fopen("Out.txt","w");

    vector<thread> pas,car;

    // Create n car threads
    for(int i=0; i<n; i++) {
        avl_cars.push_back(i+1);
        car.push_back(thread(car_func,i+1));
    }

    // Create p passenger threads
    for(int i=0; i<p; i++) {
        pas.push_back(thread(pas_func,i+1));
    }

    // Wait for all threads to finish and join
    for(int i=0;i<p;i++) {
        pas[i].join();
    }
    for(int i=0;i<n;i++) {
        car[i].join();
    }

    // Close output file
    fclose(f_out);

    // Calculate average time to complete the tour for passengers and average riding time for cars
    double avg_pass = (1.0*pas_total)/p, avg_car = (1.0*car_total)/n;

    cout << "Average time to complete the tour for passengers: " << avg_pass << " seconds" << endl;
    cout << "Average riding time for cars: " << avg_car << " seconds" << endl;

    return 0;
}