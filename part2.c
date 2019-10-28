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
// For example, create_o3 waits on g_sig_o3, which must be signaled by
// create_o2.
// g_sig_basic must be held whenever any molecule is being formed
// (including basic N and O atoms).
// You don't have to use these, if there's a different method you'd prefer.

sem_t g_sig_basic = {0};
sem_t g_sig_o2 = {0};
sem_t g_sig_n2 = {0};
sem_t g_sig_no2 = {0};
sem_t g_sig_o3 = {0};

/* These semaphores keep track of how many molecules of o2 and n2 */

sem_t g_sig_n2_moles = {0};
sem_t g_sig_o2_moles = {0};

/* These mutexes should be held when accessing the number of
 * atoms/ molecules */

pthread_mutex_t g_oxygen_mutex = {0};
pthread_mutex_t g_nitrogen_mutex = {0};
pthread_mutex_t g_o2_mutex = {0};
pthread_mutex_t g_n2_mutex = {0};

/*
 * Create oxygen atoms. The number of atoms to create is specified by the
 * argument, which is an int cast to (void *).
 */

void *create_oxygen(void *ptr) {
  int how_many = *((int *) ptr);
  free(ptr);
  ptr = NULL;


  sem_wait(&g_sig_basic);

  g_num_oxygen += how_many;
  for (int i = 1; i <= how_many; i++) {
    printf("An atom of oxygen was created.\n");

    /* Let everything know that there are enough O atoms to
     * make an o2 molecule */

    if (i % 2 == 0) {
      sem_post(&g_sig_o2);
    }
  }

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

  sem_wait(&g_sig_basic);

  g_num_nitrogen += how_many;
  for (int i = 1; i <= how_many; i++) {
    printf("An atom of nitrogen was created.\n");

    /* Let everything know that 2 Nitrogen molecules were made */

    if (i % 2 == 0) {
      sem_post(&g_sig_n2);
    }
  }

  sem_post(&g_sig_basic);

  pthread_exit(0);
} /* create_nitrogen() */

/*
 * Form N2 molecules, by combining nitrogen atoms.
 */

void *create_n2(void *ptr) {
  UNUSED(ptr);

  while (1) {

    /* Waits until a nitrogen molecule can be executed */

    sem_wait(&g_sig_n2);

    sem_wait(&g_sig_basic);

    /* Create a n2 molecule */

    g_num_nitrogen -= 2;
    g_num_n2++;

    printf("Two atoms of nitrogen combined to produce one molecule of N2.\n");

    /* If the criteria for a no2 molecule exist, signal no2 */

    if ( (g_num_n2 >= 1) && (g_num_o2 >= 2)) {
      sem_post(&g_sig_no2);
    }

    /* Ensure that there are enough nitrogen atoms to continue execution */

    int exit = g_num_nitrogen < 2;
    //printf("Num nitr: %d\n", g_num_nitrogen);

    sem_post(&g_sig_basic);

    if (exit) {
      break;
    }
  }

  pthread_exit(0);
} /* create_n2() */

/*
 * Form O2 molecules, by combining oxygen atoms.
 */

void *create_o2(void *ptr) {
  UNUSED(ptr);

  while (1) {
    // Add your code to consume two O atoms and produce one O2 molecule


    /* Wait until there are enough O atoms to make an o2 molecule */

    sem_wait(&g_sig_o2);

    sem_wait(&g_sig_basic);

    g_num_oxygen -= 2;
    g_num_o2++;

    printf("Two atoms of oxygen combined to produce one molecule of O2.\n");

    /* If there are enough o2 molecules to make o3, signal o3 */

    if (g_num_o2 >= 3) {
      sem_post(&g_sig_o3);
    }

    if ((g_num_o2 >= 2) && (g_num_n2 >= 1)) {
      sem_post(&g_sig_no2);
    }

    int exit = g_num_oxygen < 2;

    sem_post(&g_sig_basic);

    if (exit) {
      break;
    }
  }

  pthread_exit(0);
} /* create_o2() */

/*
 * Form NO2 molecules, by combining N2 and O2 molecules.
 */

void *create_no2(void *ptr) {
  UNUSED(ptr);

  while (1) {
    // Add your code to consume one N2 molecule and two O2 molecules and
    // produce two NO2 molecules


    /* Wait for one n2 molecule and two o2 molecules */

    sem_wait(&g_sig_no2);

    sem_wait(&g_sig_basic);

    /* Create a no2 molecule */

    g_num_o2 -= 2;
    g_num_n2--;

    printf("One molecule of N2 and two molecules of O2 combined to "
           "produce two molecules of NO2.\n");

    int exit = (g_num_n2 < 1) || (g_num_o2 < 2);

    sem_post(&g_sig_basic);

    if (exit) {
      break;
    }
  }

  pthread_exit(0);
} /* create_no2() */

/*
 * Form O3 molecules, by combining O2 molecules.
 */

void *create_o3(void *ptr) {
  UNUSED(ptr);

  while (1) {
    // Add your code to consume three O2 molecules and produce two O3 molecules


    /* Wait for three o2 molecules to form */

    sem_wait(&g_sig_o3);

    sem_wait(&g_sig_basic);

    g_num_o2 -= 3;

    printf("Three molecules of O2 combined to produce "
        "two molecules of O3.\n");

    int exit = g_num_o2 < 3;

    sem_post(&g_sig_basic);

    if (exit) {
      break;
    }
  }

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
  sem_init(&g_sig_o2_moles, 0, 0);
  sem_init(&g_sig_n2_moles, 0, 0);
  sem_init(&g_sig_o3, 0, 0);
  sem_init(&g_sig_no2, 0, 0);

  // Add your code to create the threads.  Remember to allocate and pass the
  // arguments for create_oxygen and create_nitrogen.


  /* Create thread for oxygen atoms */

  pthread_t oxygen_atoms_thrd = 0;
  int *atoms = (int *) malloc(sizeof(int));
  *atoms = num_oxygen;
  pthread_create(&oxygen_atoms_thrd, NULL, (void * (*)(void *)) create_oxygen,
      (void *) atoms);

  /* Create thread for nitrogen atoms */

  pthread_t nitrogen_atoms_thrd = 0;
  atoms = (int *) malloc(sizeof(int));
  *atoms = num_nitrogen;
  pthread_create(&nitrogen_atoms_thrd, NULL, (void * (*)(void *)) create_nitrogen,
      (void *) atoms);

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
}
