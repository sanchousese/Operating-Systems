// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <process.h>
#include <windows.h>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>

using namespace std;

#define BUFFER_SIZE 40
#define CONSUMERS_AMOUNT 5
#define PRODUCERS_AMOUNT 2
#define MAX_RAND_INT 1000
#define MIN_RAND_INT 100

HANDLE mutex;
HANDLE fillCount;
HANDLE emptyCount;

queue<int> buffer;


int getRandom()
{
	return abs(rand() % (MAX_RAND_INT - MIN_RAND_INT) + MIN_RAND_INT);
}


void Producer(void* arg){
	int producerIndex = (int)arg;
//	printf("Producer %d was created\n", producerIndex);

	//while (1){
	for (int i = 0; i < 100; i++){
		int item = (rand() % 100) + 1;
		printf("Producer %d was succesfully create item %d\n", producerIndex, item);
		WaitForSingleObject(emptyCount, INFINITE);
			WaitForSingleObject(mutex, INFINITE);
				buffer.push(item);
				printf("Producer %d was succesfuly put item %d to the buffer in index %d\n", producerIndex, item, buffer.size());
			ReleaseMutex(mutex);
		ReleaseSemaphore(fillCount, 1, NULL);

		Sleep(getRandom() % 3);
	}
}


void Consumer(void* arg){
	int consumerIndex = (int)arg;
//	printf("\tConsumer %d was created\n", consumerIndex);
	//while (1){
	for (int i = 0; i < 10; i++){
		WaitForSingleObject(fillCount, INFINITE);
			WaitForSingleObject(mutex, INFINITE);
				Sleep(10 * getRandom() % 3);
				int item = buffer.front();
				buffer.pop();
				printf("\tConsumer %d was succesfuly remove item %d from the buffer %d\n", consumerIndex, item, buffer.size());
			ReleaseMutex(mutex);
		ReleaseSemaphore(emptyCount, 1, NULL);
		Sleep(getRandom() % 3);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(NULL));

	mutex = CreateMutex(NULL, FALSE, NULL);
	emptyCount = CreateSemaphore(NULL, BUFFER_SIZE - 1, BUFFER_SIZE - 1, NULL);
	fillCount = CreateSemaphore(NULL, 0, BUFFER_SIZE - 1, NULL);

	for (int i = 0; i < PRODUCERS_AMOUNT; i++)
		_beginthread(Producer, 1024, (void*)i);

	for (int i = 0; i < CONSUMERS_AMOUNT; i++)
		_beginthread(Consumer, 1024, (void*)i);

	system("PAUSE");
	return 0;
}

