#include "part1.h"

#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SUCCESS (0)
#define BLOCK (-1)

// The producers add the characters in g_prod_str to the g_buffer, in order.

char *g_prod_str = "The greatest teacher, failure is.";

bounded_buffer g_buffer = {0};

// This mutex must be held whenever you use the g_buffer.

pthread_mutex_t g_buffer_mutex = {0};

// g_empty_sem is the number of characters in the g_buffer,
// that need to be emptied.
// Producers should signal g_empty_sem and consumers should wait on it.

sem_t g_empty_sem = {0};

// g_full_sem is the opposite of g_empty_sem. It is the number of
// unfilled slots in the g_buffer that need to be filled. Consumers
// should signal it, and producers should wait on it.

sem_t g_full_sem = {0};


/*
 * This function is used to queue a new character to the buffer.
 */

int enqueue(int index) {
  int value = SUCCESS;

  /* Wait until there is space in the buffer to place another character */

  sem_wait(&g_full_sem);
  pthread_mutex_lock(&g_buffer_mutex);

  g_buffer.buf[g_buffer.tail] = g_prod_str[index];
  g_buffer.tail = (g_buffer.tail + 1) % BUF_SIZE;

  pthread_mutex_unlock(&g_buffer_mutex);

  /* Let everything know that there is another character in the buffer */

  sem_post(&g_empty_sem);

  return value;
} /* enqueue() */


/*
 * This function is used to dequeue a character from the buffer.
 */

int dequeue() {

  /* Wait until there are characters to be consumed */

  sem_wait(&g_empty_sem);
  pthread_mutex_lock(&g_buffer_mutex);

  int value = g_buffer.buf[g_buffer.head];
  g_buffer.head = (g_buffer.head + 1) % BUF_SIZE;

  pthread_mutex_unlock(&g_buffer_mutex);

  /* Let everything know that there is another space available in
   * the buffer to be used */

  sem_post(&g_full_sem);

  return value;
} /* dequeue() */

/*
 * Produce the characters (that is, add them to the g_buffer) from g_prod_str,
 *  in order. Signal consumers appropriately after each character.
 * Receive an ID via an (int *).
 */

void *producer(void *ptr) {
  int thread_id = *((int *) ptr);
  free(ptr);
  ptr = NULL;

  printf("Producer %d starting\n", thread_id);
  fflush(NULL);

  for (size_t i = 0; i < strlen(g_prod_str); i++) {

    /* Insert into the queue */

    enqueue(i);

    printf("Thread %d produced %c\n", thread_id, g_prod_str[i]);
  }

  pthread_exit(0);
} /* producer() */

/*
 * Consume characters from the g_buffer. Stop after consuming the length of
 * g_prod_str, meaning that if an equal number of consumers and producers are
 * started, they will all exit. Signal producers appropriately of new free
 * space in the g_buffer. Receive and ID as an argument via an (int *).
 */

void *consumer(void *ptr) {
  int thread_id = *((int *) ptr);
  free(ptr);
  ptr = NULL;

  printf("Consumer %d starting\n", thread_id);
  fflush(NULL);

  for (size_t i = 0; i < strlen(g_prod_str); i++) {

    int value = BLOCK;

    value = dequeue();

    printf("Thread %d consumed %c\n", thread_id, value);
  }

  pthread_exit(0);
} /* consumer() */


/*
 * Start a number of producers indicated by the first argument, and a number
 * of consumers indicated by the second argument.
 * Wait on all threads at the end to prevent premature exit.
 */

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Please pass two arguments.\n");
    exit(1);
  }

  // Initialize the g_buffer_mutex and condition variables

  pthread_mutex_init(&g_buffer_mutex, NULL);

  // Since the g_buffer starts out with no characters,
  // g_empty_sem should be 0 and g_full_sem should be the full
  // size of the g_buffer.

  sem_init(&g_empty_sem, 0, 0);
  sem_init(&g_full_sem, 0, BUF_SIZE);

  /* This is used to create the thread id's */

  int num_thrds = 0;

  /* Create all of the threads */

  int num_producers = atoi(argv[1]);
  pthread_t *producers = (pthread_t *)
    malloc(num_producers * sizeof(pthread_t));

  int num_consumers = atoi(argv[2]);
  pthread_t *consumers = (pthread_t *)
    malloc(num_consumers * sizeof(pthread_t));

  for (int i = 0; i < num_producers; i++) {
    pthread_t prod_thrd = 0;
    pthread_t cons_thrd = 0;
    int *prod_id = (int *) malloc(sizeof(int));
    int *cons_id = (int *) malloc(sizeof(int));
    *prod_id = num_thrds++;
    *cons_id = *prod_id;
    producers[i] = prod_thrd;
    consumers[i] = cons_thrd;
    pthread_create(&producers[i], NULL, (void * (*)(void *)) producer,
        (void *) prod_id);
    pthread_create(&consumers[i], NULL, (void * (*)(void *)) consumer,
        (void *) cons_id);
    pthread_join(producers[i], NULL);
    pthread_join(consumers[i], NULL);
  }

  pthread_mutex_destroy(&g_buffer_mutex);
  sem_destroy(&g_full_sem);
  sem_destroy(&g_empty_sem);

  return 0;
} /* main() */
