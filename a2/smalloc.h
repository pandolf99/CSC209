
/* Remember that header files must not contain code or declaration of
 * variables that would require memory to be allocated.
 */


struct block {
    void *addr; /*start address of memory for this block */
    int size;
    struct block *next;
};

/****************************************************************************/
/* Implemented in smalloc.c */

/* Allocates size bytes of memory for the dynamically allocated memory
 * algorithm to use */
void mem_init(int size);

/* Reserves nbytes of space from the memory region created by mem_init.  Returns
 * a pointer to the reserved memory. Returns NULL if memory cannot be allocated */
void *smalloc(unsigned int nbytes);

/* Free the reserved space starting at addr.  Returns 0 if successful
 * -1 if the address cannot be found in the list of allocated blocks */
int sfree(void *addr);

/* Free any dynamically used memory in the allocated and free list */
void mem_clean();


/****************************************************************************/
/* Implemented in testhelpers.c */
/* The remaining functions are for testing purposes*/

/* prints the list of allocated blocks */
void print_allocated(void);

/* prints the list of free blocks */
void print_free(void);

/* A function that writes to the memory allocated by smalloc */
void write_to_mem(int size, char *ptr, char value);

/* prints the contents of allocated memory */
void print_mem(void);

/****************************************************************************/
/* Implemented linked_list_ops.c */
/* Operations on linked lists to make implementing smalloc easier.*/

/*
inserts a new block at the beginning of list, with relevant data.
*/
void prepend_new(struct block **l, void *addr, int s);

/*
inserts an existing unitary block to the beggining of l.
*/
void prepend_ex(struct block **l, struct block *cn);

/*
Given a pointer to a node in l, remove it from l.
*/
void remove_n(struct block **l, struct block *cn);

/*
insert cn in l after the node with address addr.
*/
void insert_middle(struct block **l, struct block *cn, void *addr);

/*
Checks if list has value addr. Returns 1 if yes, 0 if no.
*/
int check_list(struct block *l, void* addr);
