#include "part2.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define UNUSED(x) (void)(x)

// The number of molecules of each type
// (O, N, O2, and N2)

int g_num_oxygen = 0;
int g_num_nitrogen = 0;
int g_num_o2 = 0;
int g_num_n2 = 0;

// The semaphores are named according to the molecule that waits on it.
// g_sig_basic must be held whenever any molecule is being formed
// (including basic N and O atoms).

sem_t g_sig_basic = {0};
sem_t g_sig_o2 = {0};
sem_t g_sig_n2 = {0};

/* These are flags that determine when things are done */

int g_oxy_done = 0;
int g_nitr_done = 0;
int g_o2_done = 0;
int g_n2_done = 0;

/*
 * This function is used to determine when all the atoms are done
 * being created.
 */

int atoms_done() {
  sem_wait(&g_sig_basic);
  int val = g_oxy_done & g_nitr_done;
  sem_post(&g_sig_basic);
  return val;
} /* atoms_done() */

/* This function is used to determine when all the basic molecules are
 * done being created.
 */

int basic_molecules_done() {
  sem_wait(&g_sig_basic);
  int val = g_o2_done & g_n2_done;
  sem_post(&g_sig_basic);
  return val;
} /* basic_molecules_done() */


void print_values() {
  printf("N: %d\nO: %d\nN2: %d\nO2: %d\n", g_num_nitrogen, g_num_oxygen, g_num_n2, g_num_o2);
}

/*
 * Create oxygen atoms. The number of atoms to create is specified by the
 * argument, which is an int cast to (void *).
 */

void *create_oxygen(void *ptr) {
  int how_many = *((int *) ptr);
  free(ptr);
  ptr = NULL;

  for (int i = 1; i <= how_many; i++) {
    sem_wait(&g_sig_basic);
    g_num_oxygen++;
    printf("An atom of oxygen was created.\n");
    sem_post(&g_sig_basic);
  }


  sem_wait(&g_sig_basic);
  printf("*****************************\nOXYGEN DONE BEING PRODUCED\n*********************************\n");
  print_values();
  g_oxy_done = 1;
  sem_post(&g_sig_basic);
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


  for (int i = 1; i <= how_many; i++) {
    sem_wait(&g_sig_basic);
    g_num_nitrogen++;
    printf("An atom of nitrogen was created.\n");
    sem_post(&g_sig_basic);
  }


  sem_wait(&g_sig_basic);
  g_nitr_done = 1;
  printf("*****************************\nNITROGEN DONE BEING PRODUCED\n*********************************\n");
  print_values();
  sem_post(&g_sig_basic);
  pthread_exit(0);
} /* create_nitrogen() */

/*
 * Form N2 molecules, by combining nitrogen atoms.
 */

void *create_n2(void *ptr) {
  UNUSED(ptr);

  while (1) {

    sem_wait(&g_sig_basic);

    /* Ensure that there are enough nitrogen atoms to create n2 */

    int exit = g_num_nitrogen < 2;
    if (!exit) {

      /* Create a n2 molecule */

      g_num_nitrogen -= 2;
      g_num_n2++;

      printf("Two atoms of nitrogen combined to produce one molecule of N2.\n");
    }

    /* Break the loop if all atoms have been created and there are no more
     * molecules to make. */

    if ((exit) && (g_nitr_done)) {
      break;
    }

    sem_post(&g_sig_basic);
  }

  printf("*****************************\nN2 DONE BEING PRODUCED\n*********************************\n");
  print_values();
  g_n2_done = 1;

  sem_post(&g_sig_basic);
  pthread_exit(0);
} /* create_n2() */

/*
 * Form O2 molecules, by combining oxygen atoms.
 */

void *create_o2(void *ptr) {
  UNUSED(ptr);

  while (1) {

    sem_wait(&g_sig_basic);

    /* Ensure that there are enough oxygen atoms to make o2 */

    int exit = g_num_oxygen < 2;
    if (!exit) {

      g_num_oxygen -= 2;
      g_num_o2++;

      printf("Two atoms of oxygen combined to produce one molecule of O2.\n");
    }


    /* Break if there are no more oxygen atoms to use and all atoms have been
     * created */

    if ((exit) && (g_oxy_done)) {
      printf("Break o2\n,\tNum o2: %d\n", g_num_o2);
      break;
    }

    sem_post(&g_sig_basic);
  }

  printf("*****************************\nO2 DONE BEING PRODUCED\n*********************************\n");
  print_values();
  g_o2_done = 1;

  sem_post(&g_sig_basic);
  pthread_exit(0);
} /* create_o2() */

/*
 * Form NO2 molecules, by combining N2 and O2 molecules.
 */

void *create_no2(void *ptr) {
  UNUSED(ptr);

  while (1) {

    sem_wait(&g_sig_basic);

    /* Ensure that there are enough molecules to make no2 */

    int exit = (g_num_n2 < 1) || (g_num_o2 < 2);

    if (!exit) {

      /* Create a no2 molecule */

      g_num_o2 -= 2;
      g_num_n2--;

      printf("One molecule of N2 and two molecules of O2 combined to "
             "produce two molecules of NO2.\n");
    }

    /* Exit when there are no more molecules to use */

    if ((exit) && (basic_molecules_done())) {
      break;
    }

    sem_post(&g_sig_basic);
  }

  printf("*****************************\nNO2 DONE BEING PRODUCED\n*********************************\n");
  print_values();


  sem_post(&g_sig_basic);
  pthread_exit(0);
} /* create_no2() */

/*
 * Form O3 molecules, by combining O2 molecules.
 */

void *create_o3(void *ptr) {
  UNUSED(ptr);

  while (1) {

    sem_wait(&g_sig_basic);

    /* Ensure that there are enough o2 molecules to make o3 */

    int exit = g_num_o2 < 3;
    if (!exit) {

      g_num_o2 -= 3;

      printf("Three molecules of O2 combined to produce "
             "two molecules of O3.\n");
    }


    /* Exit when there are no more o3 molecules to be used */

    if ((exit) && (g_o2_done == 1)) {
      break;
    }

    sem_post(&g_sig_basic);
  }

  printf("*****************************\nO3 DONE BEING PRODUCED\n*********************************\n");
  print_values();


  sem_post(&g_sig_basic);
  pthread_exit(0);
} /* create_o3() */


/*
 * Create threads to run each chemical reaction. Wait on all threads, even
 * though many won't exit, to avoid any premature exit. The number of oxygen
 * atoms to be created is specified by the first command-line argument, and the
 * number of nitrogen atoms with the second.
 */

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Please pass two arguments.\n");
    exit(1);
  }


  int num_oxygen = atoi(argv[1]);
  int num_nitrogen = atoi(argv[2]);

  // The semaphores should all be initialized to 0, except for g_sig_basic,
  // since there is not enough material to construct any molecules except
  // for the basic atoms

  sem_init(&g_sig_basic, 0, 1);
  sem_init(&g_sig_n2, 0, 0);
  sem_init(&g_sig_o2, 0, 0);

  // Add your code to create the threads.  Remember to allocate and pass the
  // arguments for create_oxygen and create_nitrogen.


  /* Create thread for oxygen atoms */

  pthread_t oxygen_atoms_thrd = 0;
  int *atoms = (int *) malloc(sizeof(int));
  *atoms = num_oxygen;
  pthread_create(&oxygen_atoms_thrd, NULL,
      (void * (*)(void *)) create_oxygen,
      (void *) atoms);

  /* Create thread for nitrogen atoms */

  pthread_t nitrogen_atoms_thrd = 0;
  atoms = (int *) malloc(sizeof(int));
  *atoms = num_nitrogen;
  pthread_create(&nitrogen_atoms_thrd, NULL,
      (void * (*)(void *)) create_nitrogen, (void *) atoms);

  /* Create thread for n2 molecules */

  pthread_t n2_moles_thrd = 0;
  pthread_create(&n2_moles_thrd, NULL, (void * (*)(void *)) create_n2, NULL);

  /* Create thread for o2 molecules */

  pthread_t o2_moles_thrd = 0;
  pthread_create(&o2_moles_thrd, NULL, (void * (*)(void *)) create_o2, NULL);

  /* Create thread for no2 molecules */

  pthread_t no2_moles_thrd = 0;
  pthread_create(&no2_moles_thrd, NULL, (void * (*)(void *)) create_no2, NULL);

  /* Create thread for o3 molecules */

  pthread_t o3_moles_thrd = 0;
  pthread_create(&o3_moles_thrd, NULL, (void * (*)(void *)) create_o3, NULL);

  // Add your code to wait till threads are complete before main
  // continues. Unless we wait we run the risk of executing an exit which will
  // terminate the process and all threads before the threads have completed.

  pthread_join(oxygen_atoms_thrd, NULL);
  pthread_join(nitrogen_atoms_thrd, NULL);
  pthread_join(n2_moles_thrd, NULL);
  pthread_join(o2_moles_thrd, NULL);
  pthread_join(no2_moles_thrd, NULL);
  pthread_join(o3_moles_thrd, NULL);


  exit(EXIT_SUCCESS);
} /* main() */
