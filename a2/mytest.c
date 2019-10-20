#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"

/*
This test program tests for various things.

First, it tests what happens
when smalloc is called and there is no memory available either because
it was asking too much, or freelist was empty. It should print an
informative message if smalloc handled the case well.

Second, it tests that all of the possible memory can be used and allocated.
Smalloc is called 10 times asking for 4 bytes with a total of 80 bytes
avaialable. Then, it frees all of the allocated in a random order.The random
order is used to make sure that freelist is always ordered regardless of what
order is allocated_list freed.

The examiner should be able to see that freelist is always ordered through the
print messages.
*/



//Randomly shuffles an array of size 10.
//Using Fisher-Yates algorithm.
void shuffle(int a[10]) {
  srand(time(0));
  for (int i = 9; i>= 0; i--) {
    int j = rand() % (i+1);
    int temp = a[i];
    a[i] = a[j];
    a[j] = temp;
  }
}


int main(void) {
  mem_init(80);

  //Test calling smalloc asking for too much memory.
  int *big = smalloc(sizeof(int)*80);
  if (big == NULL) {
    printf("Tried to call smalloc asking for too much memory.\n");
  }


  int *arr[10];
  //Call smalloc 10 times and assign the value i + 5.
  //So freelist should be empty after this.
  for (int i = 0; i < 10; i++) {
    arr[i] = smalloc(sizeof(int));
    *arr[i] = i + 5;
  }

  printf("List of allocated blocks:\n");
  print_allocated();
  printf("List of free blocks:\n");
  print_free();
  printf("Contents of allocated memory:\n");
  print_mem();

  //Test calling smalloc when empty freelist
  void *try = smalloc(sizeof(int));
  if (try == NULL) {
    printf("Tried to call smalloc when no memory was available.\n");
  }

  //Generate a random permutation of the 10 indices.
  int indices[10] = {0,1,2,3,4,5,6,7,8,9};
  shuffle(indices);

  //Free all of allocated_list in random order.
  //freelist should be always ordered.
  for (int i = 0; i < 10; i ++) {
    printf("freeing %p result = %d\n", arr[indices[i]], sfree(arr[indices[i]]));
  }


  printf("List of allocated blocks:\n");
  print_allocated();
  printf("List of free blocks:\n");
  print_free();
  printf("Contents of allocated memory:\n");
  print_mem();

  mem_clean();
  return 1;
}
