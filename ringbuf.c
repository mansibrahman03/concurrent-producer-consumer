/*
 * Producer/consumer program using a ring buffer (lab 5).
 *
 * Team members:
 *
 *      Mansib Rahman <mrahman@cs.hmc.edu>
 *      Nina Luo <niluo@cs.hmc.edu>
 */

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int     main(int argc, char *argv[]);
void *  producer(void * arg);
void *  consumer(void * arg);
void    thread_sleep(unsigned int ms);

#define BUFSLOTS        10

/*
 * Structure used to hold messages between producer and consumer.
 */
struct message {
    int value;          /* Value to be passed to consumer */
    int consumer_sleep; /* Time (in ms) for consumer to sleep */
    int line;           /* Line number in input file */
    int print_code;     /* Output code; see below */
    int quit;           /* Nonzero if consumer should exit ("value" ignored) */
};

/*
 * The ring buffer itself.
 */
static struct message   buffer[BUFSLOTS];


 int producerNextIndex;
 int consumerNextIndex;
 int runningSum;
 int num_occupied;


static pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t bufferNotFull = PTHREAD_COND_INITIALIZER;
static pthread_cond_t bufferNotEmpty = PTHREAD_COND_INITIALIZER;


int main(int argc, char * argv[])
{
    pthread_t           consumer_tid;
    int                 error;
    pthread_mutexattr_t attr;

    /*
     * Make sure output appears right away.
     */
    setlinebuf(stdout);

    /*
     * Create an error-checking mutex.  Unfortunately, standard
     * pthread mutexes might or might not check for errors, so it's
     * necessary to initialize them by hand using a clumsy interface.
     *
     * If you are initializing multiple mutexes, you only need to do
     * the first two "mutexattr" lines once.
     */
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex, &attr);


    /*
     * Create a thread for the consumer.
     */
    error = pthread_create(&consumer_tid, NULL, consumer, NULL);
    if ((error != 0)) {
        fprintf(stderr, "Couldn't create consumer thread on line %d: %s\n",
          __LINE__, strerror(error));
        exit(1);;
    }

    /*
     * We will call the producer directly.  (Alternatively, we could
     * spawn a thread for the producer, but then we would have to join
     * it.)
     */
    producer(NULL);

    /*
     * The producer has terminated.  Clean up the consumer, which might
     * not have terminated yet.
     */
    error = pthread_join(consumer_tid, NULL);
    if (error != 0) {
        fprintf(stderr, "Couldn't join with consumer thread on line %d: %s\n",
          __LINE__, strerror(error));
        exit(1);;
    }
    return 0;
}

void * producer(void * arg)
{
    unsigned int        consumer_sleep; /* Space for reading in data */
    int                 line = 0;       /* Line number in input */
    int                 print_code;     /* Space for reading in data */
    unsigned int        producer_sleep; /* Space for reading in data */
    int                 value;          /* Space for reading in data */

    while (scanf("%d%u%u%d",
        &value, &producer_sleep, &consumer_sleep, &print_code)
      == 4) {
        line++;
        thread_sleep(producer_sleep);
        pthread_mutex_lock(&mutex);

        while (num_occupied >= BUFSLOTS) {
            pthread_cond_wait(&bufferNotFull, &mutex);
        }

        buffer[producerNextIndex].value = value;
        buffer[producerNextIndex].consumer_sleep = consumer_sleep;
        buffer[producerNextIndex].line = line;
        buffer[producerNextIndex].print_code = print_code;
        buffer[producerNextIndex].quit = 0;
        producerNextIndex = (producerNextIndex + 1) % 10;
        num_occupied++;

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&bufferNotEmpty);

        /*
         * After sending values to the consumer, print what was asked.
         */
        if (print_code == 1  ||  print_code == 3)
            printf("Produced %d from input line %d\n", value, line);
    }
    
    pthread_mutex_lock(&mutex);

    while (num_occupied >= BUFSLOTS) {
        pthread_cond_wait(&bufferNotFull, &mutex);
    }
    buffer[producerNextIndex].quit = 1;
    num_occupied++;
    producerNextIndex = (producerNextIndex + 1) % BUFSLOTS;
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&bufferNotEmpty);
    return NULL;
}

void * consumer(void * arg)
{
    /*
     * To make the output match the sample output for testinput2.txt, we
     * need to make sure the producer can run ahead of us before we consume
     * the first item.  We do that by sleeping for 100 ms.
     */
    thread_sleep(100);

    int value;
    int consumer_sleep;
    int line;

    while (1) {
        pthread_mutex_lock(&mutex);

        while (num_occupied <= 0) {
            pthread_cond_wait(&bufferNotEmpty, &mutex);
        }

        if (buffer[consumerNextIndex].quit == 0) {
            value = buffer[consumerNextIndex].value;
            line = buffer[consumerNextIndex].line;
            consumer_sleep = buffer[consumerNextIndex].consumer_sleep;
            consumerNextIndex = (consumerNextIndex+1) % BUFSLOTS;
            num_occupied--;
            pthread_mutex_unlock(&mutex);
            pthread_cond_signal(&bufferNotFull);
            thread_sleep(consumer_sleep);
            runningSum += value;
            printf("Consumed %d from input line %d; sum = %d\n", value, line, runningSum);
            } else {
                printf("Final sum is %d\n", runningSum);
                break;
        }
    }
    return NULL;
}

void thread_sleep(unsigned int ms)
{
    struct timespec     sleep_time;

    if (ms == 0)
        return;
    sleep_time.tv_sec = ms / 1000;
    sleep_time.tv_nsec = (ms % 1000) * (10^6);
    if (nanosleep(&sleep_time, NULL) == -1) {
        fprintf(stderr, "nanosleep failed on line %d: %s\n",
          __LINE__, strerror(errno));
        exit(1);
    }
}
