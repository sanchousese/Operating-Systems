#include <pthread.h>
#include <stdio.h>
#include <semaphore.h> 
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <unistd.h>

#define LIMIT_WRITERS 1
#define LIMIT_READERS 10
#define WRITERS_NUMBER 2
#define READERS_NUMBER 20


bool is_reserved_by_readers, is_writer_waiting;
int counter_reader, counter_writer;

//Для синхронізації доступу до ресурсів
sem_t x, y, z;

// Блокують потоки письменників та читачів
sem_t writer_sem, reader_sem;

//Обмежують кількісно письменників та читачів
sem_t write_sem, read_sem;

void *reader(void *arg){
	while(true){
		sleep(rand()%2 + 1);

		//Доступ до семафорів також потрібно синхронізувати, проблема була саме в цьому
		sem_wait(&z);
			//Заборона входу читачів, якщо хоча б один письменник хоче війти
			sem_wait(&reader_sem);
				//Обмежує кількість читачів
				sem_wait(&read_sem);
				sem_wait(&x);
					counter_reader++;
					//Блокуємо загальний ресурс, якщо читач перший
					if(counter_reader == 1)
						sem_wait(&writer_sem);
				sem_post(&x);
			sem_post(&reader_sem);
		sem_post(&z);

		//ЧИТАННЯ
		printf("\tReading by semaphore %ld start\n", (long) arg);
		sleep(rand()%2 + 1);
		printf("\t\tReading by semaphore %ld stop\n", (long) arg);
		//КІНЕЦЬ ЧИТАННЯ

		sem_wait(&x);
			//Відпускаємо семафор із кількістю читачів
			sem_post(&read_sem);
			counter_reader--;
			//Якщо читач останній - відпускаємо ресурс
			if(counter_reader == 0)
				sem_post(&writer_sem);
		sem_post(&x);

	}
}

void *writer(void *arg){
	while(true){
		sleep(rand()%6 + 4);

		//Додаємо письменника в чергу, якщо він перший - забороняємо читачам входити
		sem_wait(&y);
			counter_writer++;
			if(counter_writer == 1)
				sem_wait(&reader_sem);
		sem_post(&y);
		

		//ЗАПИС
		sem_wait(&writer_sem);
			printf("==============================\n");
			printf("Writing by semaphore %ld start\n", (long) arg);
			sleep(rand()%2 + 1);
			printf("Writing by semaphore %ld stop\n", (long) arg);
			printf("==============================\n");
		sem_post(&writer_sem);
		//КІНЕЦЬ ЗАПИСУ

		//Відпускаємо читачів
		sem_wait(&y);
			counter_writer--;
			if(counter_writer == 0)
				sem_post(&reader_sem);
		sem_post(&y);
	}
}

int main(int argc, char const *argv[]){
	srand (time(NULL));

	is_reserved_by_readers = false;
	is_writer_waiting = false;

	pthread_t writers[WRITERS_NUMBER];
	pthread_t readers[READERS_NUMBER];

	counter_reader = 0;
	counter_writer = 0;
	
	sem_init(&writer_sem, 0, 1);
	sem_init(&reader_sem, 0, 1);
	sem_init(&x, 0, 1);
	sem_init(&y, 0, 1);
	sem_init(&z, 0, 1);

	sem_init(&write_sem, 0, LIMIT_WRITERS);
	sem_init(&read_sem, 0, LIMIT_READERS);


	for (int i = 0; i < WRITERS_NUMBER; ++i)
		pthread_create(&writers[i], NULL, writer, (void *) i);
	
	for (int i = 0; i < READERS_NUMBER; ++i)
		pthread_create(&readers[i], NULL, reader, (void *) i);

	for (int i = 0; i < WRITERS_NUMBER; ++i)
		pthread_join(writers[i], NULL);

	for (int i = 0; i < READERS_NUMBER; ++i)
		pthread_join(readers[i], NULL);

	return 0;
}