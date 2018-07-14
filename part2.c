#include "part2.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// The number of molecules of each type
// (O, N, O2, and N2)

int num_oxygen;
int num_nitrogen;
int num_o2;
int num_n2;

// The semaphores are named according to the molecule that waits on it.
// For example, create_o3 waits on sig_o3, which must be signaled by create_o2.
// sig_basic must be held whenever any molecule is being formed
// (including basic N and O atoms).
// You don't have to use these, if there's a different method you'd prefer.

sem_t sig_basic;
sem_t sig_o2;
sem_t sig_n2;
sem_t sig_no2;
sem_t sig_o3;

/*
 * Create oxygen atoms. The number of atoms to create is specified by the
 * argument, which is an int cast to (void *).
 */

void *create_oxygen(void *ptr) {
  int how_many = *((int *) ptr);
  free(ptr);
  ptr = NULL;

  // Add your code to create how_many nitrogen atoms

  // Print this for each atom created:

  printf("An atom of oxygen was created.\n");

  // Add your code to signal consumers of oxygen

  }

  pthread_exit(0);
} /* create_oxygen() */

/*
 * Create nitrogen atoms. The number of atoms to create is specified by the
 * argument, which is an int cast to (void *).
 */

void *create_nitrogen(void *ptr) {
  int how_many = *((int *) ptr);
  free(ptr);
  ptr = NULL;

  // Add your code to create how_many nitrogen atoms

  // Print this for each atom created:

  printf("An atom of nitrogen was created.\n");

  pthread_exit(0);
} /* create_nitrogen() */

/*
 * Form N2 molecules, by combining nitrogen atoms.
 */

void *create_n2(void *ptr) {
  while (1) {
    // Add your code to wait for the proper signal.

    // Add your code to check if you can create an N2 molecule
    // and if so, adjust counts of N and N2

    printf("Two atoms of nitrogen combined to produce one molecule of N2.\n");

    // Add your code to signal consumers of N2

  }
} /* create_n2() */

/*
 * Form O2 molecules, by combining oxygen atoms.
 */

void *create_o2(void *ptr) {
  while (1) {
    // Add your code to consume two O atoms and produce one O2 molecule

    printf("Two atoms of oxygen combined to produce one molecule of O2.\n");
  }
} /* create_os() */

/*
 * Form NO2 molecules, by combining N2 and O2 molecules.
 */

void *create_no2(void *ptr) {
  while (1) {
    // Add your code to consume one N2 molecule and two O2 molecules and
    // produce two NO2 molecules

    printf("One molecule of N2 and two molecules of O2 combined to produce two molecules of NO2.\n");
  }
} /* create_no2() */

/*
 * Form O3 molecules, by combining O2 molecules.
 */

void *create_o3(void *ptr) {
  while (1) {
    // Add your code to consume three O2 molecules and produce two O3 molecules

    printf("Three molecules of O2 combined to produce two molecules of O3.\n");
  }
} /* create_o3() */


/*
 * Create threads to run each chemical reaction. Wait on all threads, even
 * though many won't exit, to avoid any premature exit. The number of oxygen
 * atoms to be created is specified by the first command-line argument, and the
 * number of nitrogen atoms with the second.
 */

int main(int argc, char **argv) {
  if (argc != 3) {
    sprintf("Please pass two arguments.");
    exit(1);
  }

  // The semaphores should all be initialized to 0, except for sig_basic,
  // since there is not enough material to construct any molecules except
  // for the basic atoms

  sem_init(&sig_basic, 0, 1);
  sem_init(&sig_n2, 0, 0);
  sem_init(&sig_o2, 0, 0);
  sem_init(&sig_o3, 0, 0);
  sem_init(&sig_no2, 0, 0);

  // Add your code to create the threads.  Remember to allocate and pass the
  // arguments for create_oxygen and create_nitrogen.



  // Add your code to wait till threads are complete before main
  // continues. Unless we wait we run the risk of executing an exit which will
  // terminate the process and all threads before the threads have completed.



  exit(EXIT_SUCCESS);
}
