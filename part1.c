#include "part1.h"

#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SUCCESS (0)
#define BLOCK (-1)

// The producers add the characters in g_prod_str to the g_buffer, in order.

char *g_prod_str = "Hello, world!";

bounded_buffer g_buffer;

// This is the number of characters in the buffer at any given time

int g_indices_produced = -1;
int g_indices_consumed = -1;

// This mutex must be held whenever you use the g_buffer.

pthread_mutex_t g_buffer_mutex;


// g_empty_sem is the number of characters in the g_buffer,
// that need to be emptied.
// Producers should signal g_empty_sem and consumers should wait on it.

sem_t g_empty_sem;

// g_full_sem is the opposite of g_empty_sem. It is the number of
// unfilled slots in the g_buffer that need to be filled. Consumers
// should signal it, and producers should wait on it.

sem_t g_full_sem;


int enqueue(char c, int index) {
  sem_wait(&g_full_sem);
  pthread_mutex_lock(&g_buffer_mutex);

/*
  if (index < g_indices_produced) {
    pthread_mutex_unlock(&g_buffer_mutex);
    return SUCCESS;
  }
*/

/*
  if (g_buffer_size == BUF_SIZE) {
    return BLOCK;
  }
*/

//  printf("--Producer: Place %c at index %d--\n", c, g_buffer.tail);

  g_buffer.buf[g_buffer.tail] = c;
  g_buffer.tail = (g_buffer.tail + 1) % BUF_SIZE;
//  g_indices_produced = index;

//  printf("----Producer: Tail is now at index %d----", g_buffer.tail);

  pthread_mutex_unlock(&g_buffer_mutex);
  sem_post(&g_empty_sem);

  return SUCCESS;
}

int dequeue(int index) {
  sem_wait(&g_empty_sem);
  pthread_mutex_lock(&g_buffer_mutex);

  int val = g_buffer.buf[g_buffer.head];
  g_buffer.head = (g_buffer.head + 1) % BUF_SIZE;

  pthread_mutex_unlock(&g_buffer_mutex);
  sem_post(&g_full_sem);

  return val;
}

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
    // Add your code to wait on the semaphore and obtain the lock,
    // then add g_prod_str[i] to the g_buffer.


    int val = enqueue(g_prod_str[i], i);

    if (val == BLOCK) {
      i--;
      continue;
    }

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
    // Add your code to wait on the semaphore and obtain the lock,
    // then consume g_prod_str[i] from the g_buffer, replacing
    // the following line.

    char c = dequeue(i);

    if (c != BLOCK) {
      printf("Thread %d consumed %c\n", thread_id, c);
    }
  }

  printf("Exit thread\n");
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

  // Remove the two following stataments when implementing your code

  (void)(argc);
  (void)(argv);

  // Initialize the g_buffer_mutex and condition variables

  pthread_mutex_init(&g_buffer_mutex, NULL);

  // Since the g_buffer starts out with no characters,
  // g_empty_sem should be 0 and g_full_sem should be the full
  // size of the g_buffer.

  sem_init(&g_empty_sem, 0, 0);
  sem_init(&g_full_sem, 0, BUF_SIZE);

  // Add your code to create the threads.
  // Make sure to allocate and pass the arguments correctly.

  /* This is used to create the thread id's */

  int num_thrds = 0;

  /* Create all of the producers */

  int num_producers = atoi(argv[1]);
  pthread_t *producers = (pthread_t *)
    malloc(num_producers * sizeof(pthread_t));
  for (int i = 0; i < num_producers; i++) {
    pthread_t thrd = 0;
    int *id = (int *) malloc(sizeof(int));
    *id = num_thrds++;
    producers[i] = thrd;
    pthread_create(&producers[i], NULL, (void * (*)(void *)) producer,
        (void *) id);
  }

  /* Create all of the consumers */

  int num_consumers = atoi(argv[2]);
  pthread_t *consumers = (pthread_t *)
    malloc(num_consumers * sizeof(pthread_t));
  for (int i = 0; i < num_consumers; i++) {
    pthread_t thrd = 0;
    int *id = (int *) malloc(sizeof(int));
    *id = num_thrds++;
    consumers[i] = thrd;
    pthread_create(&consumers[i], NULL, (void * (*)(void *)) consumer,
        (void *) id);
  }

  // pthread_create(&thrd1, NULL, (void * (*)(void *)) producer, (void *) thrd_id_1);
  // pthread_create(&thrd2, NULL, (void * (*)(void *)) consumer, (void *) thrd_id_2);

  // Add your code to wait for the threads to finish.
  // Otherwise main might run to the end
  // and kill the entire process when it exits.

  printf("Join producers\n");
  for (int i = 0; i < num_producers; i++) {
    pthread_join(producers[i], NULL);
  }

  printf("Join Consumers\n");
  for (int i = 0; i < num_consumers; i++) {
//    printf("--Join consumer %d\n", i);
    pthread_join(consumers[i], NULL);
  }

  printf("Joined\n");

  pthread_mutex_destroy(&g_buffer_mutex);

  return 0;
} /* main() */
