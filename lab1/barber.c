#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define CHAIRS 4
#define CLIENTS_AMOUNT 10

#define MAX_RAND_INT 100
#define MIN_RAND_INT 0

int queue[CHAIRS];
int pointer = -1;
int done_clients = 0;
int online_clients_value;

sem_t online_clients;
pthread_mutex_t queue_locker;
pthread_mutex_t sem_value_locker;

pthread_t customers_threads[CLIENTS_AMOUNT];
pthread_t barber_thread;

int get_online_clients();

void wakeUpBarber();

int getRandom();

void *barber(void *args) {
    printf("Barber is sleeping...\n");
    while (1) {
        sem_wait(&online_clients);
        printf("Barber is looking on queue size: %d...\n", pointer + 1);

        pthread_mutex_lock(&queue_locker);
        pointer--;
        printf("Barber shaved customer, done customers: %d\n", ++done_clients);
        if (pointer < 0)
            printf("Barber is sleeping...\n");
        pthread_mutex_unlock(&queue_locker);

        sleep(getRandom() % 3);
    }
}

void *customer(void *args) {
    int num = (int) args;
    sleep(getRandom() % 5);

    printf("Client came %d\n", num);

    pthread_mutex_lock(&sem_value_locker);

    int onlineClients = get_online_clients();

    if (pointer == -1) {
        wakeUpBarber();
    }

    if (onlineClients < CHAIRS) {
        pthread_mutex_lock(&queue_locker);
        printf("Client on chair now... %d\n", num);
        sem_post(&online_clients);


        queue[pointer++] = num;
        printf("Queue size: %d\n", pointer + 1);
        pthread_mutex_unlock(&queue_locker);
    }
    else {
        printf("Client go out %d\n", num);
    }

    pthread_mutex_unlock(&sem_value_locker);
}


// utils
void wakeUpBarber() {
    printf("-----Wake up barber\n");
}

int get_online_clients() {
    sem_getvalue(&online_clients, &online_clients_value);
    return online_clients_value;
}

int getRandom() {
    return rand() % (MAX_RAND_INT - MIN_RAND_INT) + MIN_RAND_INT;
}


// main
int main() {
    int i;
    srand(time(NULL));

    sem_init(&online_clients, 0, 0);
    pthread_mutex_init(&queue_locker, NULL);
    pthread_mutex_init(&sem_value_locker, NULL);


    pthread_create(&barber_thread, NULL, barber, NULL);

    for (i = 0; i < CLIENTS_AMOUNT; ++i) {
        pthread_create(&(customers_threads[i]), NULL, customer, (void *) i);
    }

    pthread_join(barber_thread, NULL);

    return 0;
}
