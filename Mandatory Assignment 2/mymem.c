#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>



/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList
{
  // doubly-linked list
  struct memoryList *last;
  struct memoryList *next;

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *next;

struct memoryList *lastAllocatedBlock = NULL;

/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz)
{
	myStrategy = strategy;

	/* all implementations will need an actual block of memory to use */
	mySize = sz;

	if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

	/* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
	myMemory = malloc(sz);

	/*release old linked list if it exists*/
	if (head != NULL){
		struct memoryList* currentBlock = head;
		while(currentBlock->next != NULL){
			free(currentBlock->last);
			currentBlock = currentBlock->next;
		}
		free(currentBlock);
	}
	//reset bookkeping
	lastAllocatedBlock = NULL;

	/* TODO: Initialize memory management structure. */

	//make new struct and make head to point to it
	//create one block of memory that is not allocated
	head = (struct memoryList*)malloc(sizeof(struct memoryList));
	head->last = NULL;
	head->next = NULL;
	head->alloc = 0;
	head->ptr = myMemory;
	head->size = sz;

}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested)
{
	assert((int)myStrategy > 0);

	//check if initialized
	//if not return NULL
	if (head == NULL){
		return NULL;
	}

	//define traversal pointer, new block pointer and other neccesary data variables all strategies use
	struct memoryList *currentblock = head;
	struct memoryList *newBlock = NULL;
	struct memoryList *tempNext = NULL;
	int leftoverSize = 0;

	switch (myStrategy)
	  {
	  case NotSet: 
	            return NULL;
	  case First: ;
		//##########Traverse memory list until location that fits strategy#############

		//move to next block until adequate block or end of list
		while(((currentblock->alloc == 1) || (currentblock->size < requested)) && (currentblock->next != NULL) ){
			currentblock = currentblock->next;
		}

		//if we are at the end of list without fullfilling memory requirements return null
		if((currentblock->alloc == 1) || (currentblock->size < requested)){
			return NULL;
		}//if not continue memory allocation

		//##########Change size of block, allocate it and create new block with remaining memory #############

		//leftover size
		leftoverSize = currentblock->size - requested;

		//set new size of the block, but keep same pointer
		currentblock->alloc = 1;
		currentblock->size = requested;

		//if it fits perfectly no need for extra block for unallocated space
		if(leftoverSize == 0){
			return currentblock->ptr;
		}
		//create new block with leftover mem
		//allocate rest of mem to new block

		newBlock = createBlock(leftoverSize, 0, (currentblock->ptr + requested));

		//##########Connecting new block to memory list #############

		insertBlock(currentblock, newBlock, tempNext);

		//return pointer to newly allocated memory
		return currentblock->ptr;

	  case Best: ;

		struct memoryList *memoryBestFit = NULL;
		int bestFitSize = 0;

		//##########Traverse memory list until location that fits strategy#############
		while(currentblock != NULL){
			if((currentblock->size >= requested) && (currentblock->alloc == 0) && (memoryBestFit == NULL)){
				memoryBestFit = currentblock;
				bestFitSize = currentblock->size;
			}else if((currentblock->size >= requested) && (currentblock->alloc == 0) && (currentblock->size < bestFitSize)){
				memoryBestFit = currentblock;
				bestFitSize = currentblock->size;
			}
			currentblock = currentblock->next;

		}

		//set the block we found as current block
		currentblock = memoryBestFit;
		
		//if we found no suitable block we return NULL
		if(currentblock == NULL){
			return NULL;
		}

		//##########Change size of block, allocate it and create new block with remaining memory #############
		//leftover size
		leftoverSize = currentblock->size - requested;

		//set new size of the block, but keep same pointer
		currentblock->alloc = 1;
		currentblock->size = requested;

		//if it fits perfectly no need for extra block for unallocated space
		if(leftoverSize == 0){
			return currentblock->ptr;
		}
		//allocate new block and give remaining mem to it
		newBlock = createBlock(leftoverSize, 0, (currentblock->ptr + requested));


		//##########Connecting new block to memory list #############
		insertBlock(currentblock, newBlock, tempNext);

		
	    return currentblock->ptr;

	  case Worst: ;

			struct memoryList *biggestBlock = NULL;
			int biggestSize = 0;

			//##########Traverse memory list until location that fits strategy#############
			//traverse whole list once to find the biggest block
			//if a block is not allocated and its size is bigger than what we have saved replace our saved block
			while(currentblock != NULL){
				if((currentblock->alloc == 0) && (currentblock->size > biggestSize)){
					biggestSize = currentblock->size;
					biggestBlock = currentblock;
				}
				currentblock = currentblock->next;
			}

			currentblock = biggestBlock;//set chosen block as current

			//could not find big enough block
			if(requested > biggestSize){
				return NULL;
			}

			//##########Change size of block, allocate it and create new block with remaining memory #############
			//leftover size
			leftoverSize = currentblock->size - requested;

			//set new size of the block, but keep same pointer
			currentblock->alloc = 1;
			currentblock->size = requested;

			//if it fits perfectly no need for extra block for unallocated space
			if(leftoverSize == 0){
				return currentblock->ptr;
			}

			//allocate new block and give remaining mem to it
			newBlock = createBlock(leftoverSize, 0, (currentblock->ptr + requested));

			//##########Connecting new block to memory list #############
			insertBlock(currentblock, newBlock, tempNext);

			//return ptr to newly allocated mem
	        return currentblock->ptr;
	  case Next: ;

		//choose starting point
		if((lastAllocatedBlock == NULL) || (lastAllocatedBlock->next == NULL)){
			currentblock = head;
		}else{
			currentblock = lastAllocatedBlock;
		}
	  	
		//##########Traverse memory list until location that fits strategy#############
		do{
			if((currentblock->size >= requested) && (currentblock->alloc == 0)){
				break;
			}
	
			//wraparound to start or proceed to next block
			if(currentblock->next == NULL){
				currentblock = head;
			}else{
				currentblock = currentblock->next;
			}
			
		}while (currentblock != lastAllocatedBlock);
	
		//could not find block
		if(currentblock == lastAllocatedBlock){
			return NULL;
		}

		//##########Change size of block, allocate it and create new block with remaining memory #############
		//leftover size
		leftoverSize = currentblock->size - requested;

		//set new size of the block, but keep same pointer
		currentblock->alloc = 1;
		currentblock->size = requested;

		//if it fits perfectly no need for extra block for unallocated space
		if(leftoverSize == 0){
			lastAllocatedBlock = currentblock;
			return currentblock->ptr;
		}
		//allocate new block and give remaining mem to it
		newBlock = createBlock(leftoverSize, 0, (currentblock->ptr + requested));



		//##########Connecting new block to memory list #############
		insertBlock(currentblock, newBlock, tempNext);



		lastAllocatedBlock = currentblock;
		//printf("last allocated block: %p\n", lastAllocatedBlock->ptr);
		
		return currentblock->ptr;

	  default:
		break;
	  }
	  
	return NULL;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
	struct memoryList* currentBlock = head;
	struct memoryList* tmpBlock = head;


	//#############FIND_BLOCK#################
	while ((currentBlock!= NULL) && (currentBlock->ptr != block) )
	{
		currentBlock = currentBlock->next;
	}

	//could not find block
	if(currentBlock == NULL){
		return;
	}

	//#############DEALLOCATE################
	//printf("deallocating %d\n", currentBlock->size);
	currentBlock->alloc = 0;


	//#############MERGING#################
	//if block behind is unallocated merge
	if ((currentBlock->last != NULL) && (currentBlock->last->alloc == 0)){

		//make block behind us new current block save old as tmp
		tmpBlock = currentBlock;
		currentBlock = currentBlock->last;
		currentBlock->size = (currentBlock->size + tmpBlock->size); //get new size

		//move nextfit pointer from tmp block
		if(tmpBlock == lastAllocatedBlock){
			lastAllocatedBlock = currentBlock;
		}

		if(tmpBlock->next == NULL){//just remove if at end of list
			currentBlock->next = NULL;
			free(tmpBlock);
		}else{//if not at end link them together
			//connect blocks forward and behind tmp
			currentBlock->next = tmpBlock->next;
			tmpBlock->next->last = currentBlock; 
			free(tmpBlock);
		}

	}
	
	//if block infront is also unallocated also merge
	if((currentBlock->next != NULL) && (currentBlock->next->alloc == 0)){
		//designate block to be merged as tmp
		tmpBlock = currentBlock->next;
		currentBlock->size = ((currentBlock->size) + (tmpBlock->size));

		//move nextfit pointer from tmp block
		if(tmpBlock == lastAllocatedBlock){
			lastAllocatedBlock = currentBlock;
		}

		if(tmpBlock->next == NULL){
			currentBlock->next = NULL;
			free(tmpBlock);
		}else{
			//connect blocks forward and behind tmp
			currentBlock->next = tmpBlock->next;
			tmpBlock->next->last = currentBlock; 
			free(tmpBlock);

		}

	}
	
	



	return;
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
	struct memoryList* currentBlock = head;
	int numOfFreeBlocks = 0;

	while(currentBlock != NULL){
		if(currentBlock->alloc == 0){
			numOfFreeBlocks++;
		}
		currentBlock = currentBlock->next;
	}

	return numOfFreeBlocks;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
	struct memoryList* currentBlock = head;
	int allocatedsum = 0;

	while(currentBlock != NULL){

		if(currentBlock->alloc == 1){
			allocatedsum = currentBlock->size + allocatedsum;
		}
		currentBlock = currentBlock->next;
	}	

	return allocatedsum;
}

/* Number of non-allocated bytes */
int mem_free()
{
	return (mem_total() - mem_allocated());
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
	struct memoryList* currentBlock = head;
	size_t LargestSize = 0;

	while(currentBlock != NULL){
		if((currentBlock->alloc == 0) && (currentBlock->size > LargestSize) ){
			LargestSize = currentBlock->size;
		}
		currentBlock = currentBlock->next;
	}

	return LargestSize;
}

/* Number of free blocks smaller than "size" bytes. */
//revidering
/* Number of free blocks smaller than or equal to"size" bytes. */
int mem_small_free(int size)
{
	struct memoryList *currentBlock = head;
	int numberOfBlocks = 0;

	while(currentBlock != NULL){
		if((currentBlock->size <= size) && (currentBlock->alloc == 0) ){
			numberOfBlocks++;
		}
		currentBlock = currentBlock->next;
	}
	return numberOfBlocks;
}       

char mem_is_alloc(void *ptr)
{
	struct memoryList *currentBlock = head;

	while(currentBlock != NULL){
		if((currentBlock->ptr <= ptr) && (ptr < (currentBlock->ptr + currentBlock->size)) ){
			return 'y';
		}

		currentBlock = currentBlock->next;
	}
    return 'n';
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
	return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy)
{
	switch (strategy)
	{
		case Best:
			return "best";
		case Worst:
			return "worst";
		case First:
			return "first";
		case Next:
			return "next";
		default:
			return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
	if (!strcmp(strategy,"best"))
	{
		return Best;
	}
	else if (!strcmp(strategy,"worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy,"first"))
	{
		return First;
	}
	else if (!strcmp(strategy,"next"))
	{
		return Next;
	}
	else
	{
		return 0;
	}
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{
	struct memoryList *node = head;

	//print all
	while(node != NULL){
		printf("size\t: %3d, allocated\t: %d, address: \t: %p\n", node->size, node->alloc, node->ptr);
		node = node->next;
	}
	return;
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status()
{
	
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
	

}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
        strategies strat;
	void *a, *b, *c, *d, *e;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */
	
	initmem(strat,500);
	
	a = mymalloc(100);
	b = mymalloc(100);
	c = mymalloc(100);
	myfree(b);
	d = mymalloc(50);
	myfree(a);
	e = mymalloc(25);
	
	//void *f = mymalloc(125);
	//void *u = mymalloc(100); 
	

/*
	a = mymalloc(100);
	b = mymalloc(100);
	c = mymalloc(100);
	d = mymalloc(100);
	e = mymalloc(100);

	
	myfree(b);
	myfree(d);

	void *f = mymalloc(100);
*/


	print_memory();
	print_memory_status();
	
}

//the function takes the node indicated as "newBlock" and places it after the location of the "currentBlock"
void insertBlock(struct memoryList *currentblock, struct memoryList *newBlock, struct memoryList *tempNext){
	
	//we first check whether current block has a next node
	if(currentblock->next == NULL){	//if not connect directly
			currentblock->next = newBlock;
			newBlock->last = currentblock;
			newBlock->next = NULL;
	}else{ // if so connect place between them
			//save current blk next in temp
			//struct memoryList *tempNext;
			tempNext = currentblock->next;

			//connect new block to current block
			currentblock->next = newBlock;
			newBlock->last = currentblock;

			//use saved pointer to connect the new block to the next block
			newBlock->next = tempNext;
			tempNext->last = newBlock;

	}
	
}

//creates a new block ot place in linkedlist and return its pointer to the newBlock var
struct memoryList* createBlock(int size, int allocation, void *pointerToMem){
		struct memoryList* newBlock = (struct memoryList*)malloc(sizeof(struct memoryList));
		newBlock->alloc = allocation;
		newBlock->ptr = pointerToMem;
		newBlock->size = size;

		return newBlock;
} 

