#include "part1.h"

#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// The producers add the characters in prod_str to the buffer, in order.

char *prod_str = "Hello, World!";

bounded_buffer buffer;

// This mutex must be held whenever you use the buffer.

pthread_mutex_t buffer_mutex;

// empty_sem is the number of characters in the buffer, that need to be emptied.
// Producers should signal empty_sem and consumers should wait on it.

sem_t empty_sem;

// full_sem is the opposite of empty_sem. It is the number of unfilled slots in
// the buffer that need to be filled. Consumers should signal it, and producers
// should wait on it.

sem_t full_sem;

/*
 * Produce the characters (that is, add them to the buffer) from prod_str, in
 * order. Signal consumers appropriately after each character. Receive an ID via
 * an (int *).
 */

void* producer(void *ptr) {
  int thread_id = *((int *) ptr);
  free(ptr);
  ptr = NULL;

  printf("Producer %d starting\n", thread_id);
  fflush(NULL);

  for (int i = 0; i < strlen(prod_str); i++) {
    // Add your code to wait on the semaphore and obtain the lock,
    // then add prod_str[i] to the buffer.

    printf("Thread %d produced %c\n", thread_id, prod_str[i]);
  }

  pthread_exit(0);
} /* producer() */

/*
 * Consume characters from the buffer. Stop after consuming the length of
 * prod_str, meaning that if an equal number of consumers and producers are
 * started, they will all exit. Signal producers appropriately of new free space
 * in the buffer. Receive and ID as an argument via an (int *).
 */

void* consumer(void *ptr) {
  int thread_id = *((int *) ptr);
  free(ptr);
  ptr = NULL;

  printf("Consumer %d starting\n", thread_id);
  fflush(NULL);

  for (int i = 0; i < strlen(prod_str); i++) {
    // Add your code to wait on the semaphore and obtain the lock,
    // then consume prod_str[i] from the buffer, replacing
    // the following line.

    char c = prod_str[i];

    printf("Thread %d consumed %c\n", thread_id, c);
  }

  pthread_exit(0);
} /* consumer() */

/*
 * Start a number of producers indicated by the first argument, and a number of
 * consumers indicated by the second argument. Wait on all threads at the end to
 * prevent premature exit.
 */

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Please pass two arguments.");
    exit(1);
  }

  // Initialize the buffer_mutex and condition variables

  pthread_mutex_init(&buffer_mutex, NULL);

  // Since the buffer starts out with no characters, empty_sem should be 0 and
  // full_sem should be the full size of the buffer.

  sem_init(&empty_sem, 0, 0);
  sem_init(&full_sem, 0, BUF_SIZE);

  // Add your code to create the threads.
  // Make sure to allocate and pass the arguments correctly.


  // Add your code to wait for the threads to finish.
  // Otherwise main might run to the end
  // and kill the entire process when it exits.



  pthread_mutex_destroy(&buffer_mutex);

  return 0;
} /* main() */
