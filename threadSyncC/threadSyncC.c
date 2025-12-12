#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex;
pthread_cond_t cond;

struct Node {
  int data;
  struct Node *next;
};

struct Node *head = NULL;

void *producer(void *arg) {
  while (1) {
    pthread_mutex_lock(&mutex);
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    new_node->data = rand() % 100;
    new_node->next = head;
    head = new_node;
    printf("thread %lu produced %d\n", (unsigned long)pthread_self(),
           new_node->data);
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&cond);
    sleep(rand() % 3);
  }
  return NULL;
}

void *consumer(void *arg) {
  while (1) {
    pthread_mutex_lock(&mutex);
    while (head == NULL) {
      pthread_cond_wait(&cond, &mutex);
    }
    struct Node *node = head;
    head = node->next;
    printf("thread %lu consumed %d\n", (unsigned long)pthread_self(),
           node->data);
    free(node);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

int main() {
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  pthread_t producer_thread[5];
  pthread_t consumer_thread[5];

  for (int i = 0; i < 5; i++) {
    pthread_create(&producer_thread[i], NULL, producer, NULL);
  }
  for (int i = 0; i < 5; i++) {
    pthread_create(&consumer_thread[i], NULL, consumer, NULL);
  }

  for (int i = 0; i < 5; i++) {
    pthread_join(producer_thread[i], NULL);
  }
  for (int i = 0; i < 5; i++) {
    pthread_join(consumer_thread[i], NULL);
  }

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);

  return 0;
}
