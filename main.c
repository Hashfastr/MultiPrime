#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>

typedef struct {
    int id;
    int startvalue;
    int max;
} Data;

clock_t t1, t2;
int *primes, *primeptr;
int quota, threadnum;

void *status;

pthread_t *threads, *threadptr;

int is_prime(int test);
void *driver(void *pass);

int main(int argc, char **argv) {

    if (argc!=3) { // make sure we actually have the right arguments to avoid a segfault
        printf("Usage: prime [number of primes to be computed] [number of threads to use]\n");
        return 1;
    }

    quota=atoi(argv[1]); // get how many primes we want to calculate
    threadnum=atoi(argv[2]);

    printf("Allocating memory...\n");
    primes=malloc(sizeof primes * (quota+1000)); // allocate the memory for our data
    primeptr=primes;

    threads=malloc(sizeof threads * (threadnum));
    threadptr=threads;

    pthread_attr_t attr;

    printf("Zeroing out the array...\n");
    for (int i=0; i<quota; i++) // zero out the array
        primeptr[i]=0; 

    primeptr[0]=2; // give us the first prime

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    t1=clock(); // start the timer

    printf("Starting computation...\n");
    //for (int i=3; count<quota; i+=2)
    //is_prime(i);

    //for (int value=3; count<quota; value+=threadnum*2) {


    for (int i=0; i<threadnum; i++) {
        // zero out the array 
        Data *data = malloc(sizeof(Data));
        data->id=i+1;
        data->startvalue=3+(i*2);
        if (i==0)
            data->max=floor(quota/threadnum)+(quota%threadnum);
        else
            data->max=floor(quota/threadnum);

        pthread_create(threadptr, &attr, driver, (void *)data);
    }

    for (int i=0; i<threadnum; i++)
        pthread_join(threadptr[i], &status);

    t2=clock(); // stop timer 

    system("sleep 1");

    pthread_attr_destroy(&attr);

    /*
       int temp;

       for (int i= 0; i < quota; i++) {
       for (int j = 0; j <= (quota - i - 1); j++) {
       if (primeptr[j] > primeptr[j + 1]) {
       temp = primeptr[j];
       primeptr[j] = primeptr[j + 1];
       primeptr[j + 1] = temp;
       }
       }
       }

       for (int i=1; i<=quota; i++)
       printf("%d. %d\n", i, primeptr[i-1]);
       */

    float secs = ((float)t2-(float)t1) / CLOCKS_PER_SEC; // translate the time to seconds



    printf("Max prime found: %d\nTime taken: %f\n", primeptr[quota-1], secs/threadnum);

    free(threads);
    free(primes);
    pthread_exit(NULL);
}

int is_prime(int test) {
    double primesqrt = sqrt(test); // get a square root of the number we are testing
    if (primesqrt==floor(primesqrt)) return 0; //make sure we aren't testing a square root

    for (int i=0; primeptr[i]!=0 && primeptr[i]<primesqrt; i++) { // loop through known primes,
        if ((test%primeptr[i])==0) // and sees if it's divisible by that prime
            return 0; // if so then return 0
    }

    for (int i=3; i<primesqrt; i+=2)
        if ((test%i)==0) 
            return 0;

    return 1;
}

void *driver(void *pass) {
    Data *data = (Data*)pass;

    int personalcount=0, splice=data->id;
    double areprime=0, arentprime=0;

    printf("Thread #%d started with value %d and quota of %d\n", data->id, data->startvalue, data->max);

    for (int i=data->startvalue; personalcount<data->max; i+=(threadnum*2)) {
        if (is_prime(i)) {
            //if (data->id != 1) pthread_join(threadptr[data->id-1], NULL);
            primeptr[splice]=i;
            //printf("thread #%d: %d. %d\n", data->id, splice, primeptr[splice]);
            splice+=threadnum;
            personalcount++;
            areprime++;
            arentprime++;
        }
        else arentprime+=2;
    }

    printf("Thread #%d had a prime ratio of %f\n", data->id, (areprime/arentprime));

    free(pass);
    pthread_exit((void*) 0);
    return 0;
}
