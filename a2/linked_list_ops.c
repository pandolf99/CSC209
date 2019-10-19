#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"

/*
Some operations over linked lists. See header file for descriptions.
*/

void prepend_new(struct block **l, void *addr, int s) {
  struct block *new_block = malloc(sizeof(struct block));
  new_block->addr = addr;
  new_block->size = s;
  new_block->next = *l;
  *l = new_block;
}


void prepend_ex(struct block **l, struct block *cn) {
  cn->next = *l;
  *l = cn;
}


void insert_middle(struct block **l, struct block *cn, void *addr) {
  struct block *curr = *l;
  while (curr->addr != addr) {
    curr = curr->next;
  }
  cn->next = curr->next;
  curr->next = cn;
}

void remove_n(struct block **l, struct block *cn) {
  if (*l == cn) {
    *l = cn->next;
  }
  else {
    struct block *curr_node = *l;
    while (curr_node->next != cn) {
      curr_node = curr_node->next;
    }
    curr_node->next = cn->next;
  }
}

int check_list(struct block *l, void* addr) {
  if (l == NULL) {
    return 0;
  }
  int val = 0;
  struct block *curr = l;
  while (curr != NULL) {
    if (curr->addr == addr) {
      val = 1;
      break;
    }
    else {
      curr = curr->next;
    }
  }
  return val;
}
