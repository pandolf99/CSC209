#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"



void *mem;
struct block *freelist;
struct block *allocated_list;


/*
  Given an integer, reurn its closest multiple of 8 in the positive direction.
*/
int mult_8(int i) {
  if (i%8 == 0) {
    return i;
  }
  else return(mult_8(i+1));
}


/*
Return the pointer to the block of freelist that the block with address adr
should be inserted into.
*/
struct block* find_right(void *addr) {
  struct block *curr_node = freelist;
  //Cases where the first block should be returned.
  if (freelist == NULL || freelist->next == NULL || addr < freelist->addr) {
    return freelist;
  }
  //Find the node with correct address so freelist is ordered.
  while (curr_node->next != NULL){
    if (curr_node->addr < addr && addr < curr_node->next->addr) {
      break;
    }
    else curr_node = curr_node->next;
  }
  return curr_node;
}



/*
  Manage the case where smalloc finds a block bigger than the requested
*/
void smalloc_1(struct block *cn, unsigned int n) {
  void *address = cn->addr;
  unsigned int size = n;

  prepend_new(&allocated_list, address, size);

  //Change values of cn
  cn->addr = cn->addr + n;
  cn->size = cn->size - n;
}

/*
  Manage the case where smalloc finds a block equal to the requested
*/
void smalloc_2(struct block *cn) {
  remove_n(&freelist, cn);
  prepend_ex(&allocated_list, cn);
}

/*
  Manage linked lists to keep track of which memory is in use and which memory
  is free.
*/
void *smalloc(unsigned int nbytes) {
  if (nbytes == 0) {
    return NULL;
  }
  nbytes = mult_8(nbytes);


  struct block *curr_node = freelist;
  if (curr_node == NULL) {
    return NULL;
  }
  //Find available memory in free List
  while (curr_node->next != NULL) {
    if (curr_node->size >= nbytes) break;
    else curr_node = curr_node->next;
  }
  // Check if there is no available memory
  if (curr_node->next == NULL && curr_node->size < nbytes) {
    return NULL;
  }
  //cases for when it finds memory.
  if (curr_node->size > nbytes) {
    smalloc_1(curr_node, nbytes);
  }
  else {
    smalloc_2(curr_node);
  }
  return allocated_list->addr;
}

/*
  Check that address is not in allocated list and is in freelist
*/
int check_free(void* addr) {
  int allo = check_list(allocated_list, addr);
  int free = check_list(freelist, addr);
  if (allo == 0 && free == 1) {
    return 0;
  }
  else return -1;
}

int sfree(void *addr) {
  //remove the right block from allocated_list.
  struct block *cn = allocated_list;
  while (cn->addr != addr) {
    cn = cn->next;
  }
  remove_n(&allocated_list, cn);
  struct block *good_n = find_right(cn->addr);

  //Check if neccessary to prepend.
  if (good_n == freelist && (freelist == NULL || freelist->addr > addr)) {
      prepend_ex(&freelist, cn);
  }
  else {
    insert_middle(&freelist, cn, good_n->addr);
  }
  //Check  freeing worked correctly.
  return check_free(addr);
}


/* Initialize the memory space used by smalloc,
 * freelist, and allocated_list
 * Note:  mmap is a system call that has a wide variety of uses.  In our
 * case we are using it to allocate a large region of memory.
 * - mmap returns a pointer to the allocated memory
 * Arguments:
 * - NULL: a suggestion for where to place the memory. We will let the
 *         system decide where to place the memory.
 * - PROT_READ | PROT_WRITE: we will use the memory for both reading
 *         and writing.
 * - MAP_PRIVATE | MAP_ANON: the memory is just for this process, and
 *         is not associated with a file.
 * - -1: because this memory is not associated with a file, the file
 *         descriptor argument is set to -1
 * - 0: only used if the address space is associated with a file.
 */
void mem_init(int size) {
    mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if(mem == MAP_FAILED) {
         perror("mmap");
         exit(1);
    }
    freelist = malloc(sizeof(struct block));
    freelist->addr = mem;
    freelist->size = size;
    freelist->next = NULL;
    allocated_list = NULL;
}

/*
Free memory of lists
*/
void free_l(struct block *l) {
  if (l == NULL) {
    free(l);
    return;
  }
  if (l->next == NULL) {
    free(l);
  }
  else {
    free_l(l->next);
    free(l);
  }
}

void mem_clean(){
  //free freelist and allocated_list
  free_l(allocated_list);
  free_l(freelist);
}
