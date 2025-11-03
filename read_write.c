#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t wrt;              // Semaphore for writers
pthread_mutex_t mutex;  // Mutex for reader count
int readcount = 0;      // Number of readers
int data = 0;           // Shared data

void *writer(void *wno) {
    int num = *(int *)wno;
    while (1) {
        sem_wait(&wrt); // Writer wants access
        data++;
        printf("Writer %d writes data = %d\n", num, data);
        sem_post(&wrt); // Release access
        sleep(2);
    }
}

void *reader(void *rno) {
    int num = *(int *)rno;
    while (1) {
        pthread_mutex_lock(&mutex);
        readcount++;
        if (readcount == 1)
            sem_wait(&wrt); // First reader locks writer
        pthread_mutex_unlock(&mutex);

        printf("Reader %d reads data = %d\n", num, data);

        pthread_mutex_lock(&mutex);
        readcount--;
        if (readcount == 0)
            sem_post(&wrt); // Last reader unlocks writer
        pthread_mutex_unlock(&mutex);

        sleep(1);
    }
}

int main() {
    pthread_t read[5], write[2];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&wrt, 0, 1);

    int reader_id[5] = {1,2,3,4,5};
    int writer_id[2] = {1,2};

    for (int i = 0; i < 5; i++)
        pthread_create(&read[i], NULL, reader, &reader_id[i]);
    for (int i = 0; i < 2; i++)
        pthread_create(&write[i], NULL, writer, &writer_id[i]);

    for (int i = 0; i < 5; i++)
        pthread_join(read[i], NULL);
    for (int i = 0; i < 2; i++)
        pthread_join(write[i], NULL);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&wrt);
}
