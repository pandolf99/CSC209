#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"

int main (void) {
  mem_init(8000);

  //Make an array of 10 of int*

  int **arr = smalloc(sizeof(int*)*10);

  printf("%p\n",arr );
  for (int i = 0; i < 10; i++) {
    arr[i] = smalloc(sizeof(int));
    *arr[i] = i + 5;
    printf("Adress: %p, Value: %d\n", arr[i], *arr[i]);
  }

  printf("List of allocated blocks:\n");
  print_allocated();
  printf("List of free blocks:\n");
  print_free();
  printf("Contents of allocated memory:\n");
  print_mem();

  for (int i = 0; i < 10; i ++) {
    printf("freeing %p result = %d\n", arr[i], sfree(arr[i]));
  }
  printf("freeing %p result = %d\n", arr, sfree(arr));

  printf("List of allocated blocks:\n");
  print_allocated();
  printf("List of free blocks:\n");
  print_free();
  printf("Contents of allocated memory:\n");
  print_mem();

  mem_clean();
}
