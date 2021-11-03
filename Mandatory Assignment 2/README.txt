 Memory Management
===================

The problem will focus on memory.  You will implement your own 
version of malloc() and free(), using a variety of allocation strategies.

You will be implementing a memory manager for a block of memory.  You will
implement routines for allocating and deallocating memory, and keeping track of
what memory is in use.  You will implement one of the four strategies for selecting in
which block to place a new requested memory block:

  1) First-fit: select the first suitable block with smallest address.
  2) Best-fit: select the smallest suitable block.
  3) Worst-fit: select the largest suitable block.
  4) Next-fit: select the first suitable block after
     the last block allocated (with wraparound
     from end to beginning).


Here, "suitable" means "free, and large enough to fit the new data".

Here are the functions you will need to implement:

initmem():
  Initialize memory structures.

mymalloc():
  Like malloc(), this allocates a new block of memory.

myfree():
  Like free(), this deallocates a block of memory.

mem_holes():
  How many free blocks are in memory?

mem_allocated():
  How much memory is currently allocated?

mem_free():
  How much memory is NOT allocated?

mem_largest_free():
  How large is the largest free block?

mem_small_free():
  How many small unallocated blocks are currently in memory?

mem_is_alloc():
  Is a particular byte allocated or not?

A structure has been provided for use to implement these functions.  It is a
doubly-linked list of blocks in memory (both allocated and free blocks).  Every
malloc and free can create new blocks, or combine existing blocks.  You may
modify this structure, or even use a different one entirely.  However, do not
change function prototypes or files other than mymem.c.

IMPORTANT NOTE: Regardless of how you implement memory management, make sure
that there are no adjacent free blocks.  Any such blocks should be merged into
one large block.

A few functions are given to help you monitor what happens when you
call your functions.  Most important is the try_mymem() function.  If you run
your code with "mem -try <args>", it will call this function, which you can use
to demonstrate the effects of your memory operations.  These functions have no
effect on test code, so use them to your advantage.

Running your code:

After running "make", run

1) "mem" to see the available tests and strategies.
2) "mem -test <test> <strategy>" to test your code with provided tests.
3) "mem -try <args>" to run your code with your own tests
   (the try_mymem function).

You can also use "make test" and "make stage1-test" for testing.  "make
stage1-test" only runs the tests relevant to stage 1.

Running "mem -test -f0 ..." will allow tests to run even
after previous tests have failed.  Similarly, using "all" for a test or strategy
name runs all of the tests or strategies.  Note that if "all" is selected as the
strategy, the 4 tests are shown as one.

One of the tests, "stress", runs an assortment of randomized tests on each
strategy.  The results of the tests are placed in "tests.out" .  You may want to
view this file to see the relative performance of each strategy.


Stage 1
-------

Implement all the above functions, for all the 4 strategy in a group
Use "mem -test all first" to test your implementation


Stage 2
-------
you should answer the 10 questions asked below together in a group.
The strategy is passed to initmem(), and stored in the global variable "myStrategy".
Some of your functions will need to check this variable to implement the
correct strategy.

You can test your code with "mem -test all worst", etc., or test all 4 together
with "mem -test all all".  The latter command does not test the strategies
separately; your code passes the test only if all four strategies pass.


Answer the following questions as part of your report
=====================================================

1) Why is it so important that adjacent free blocks not be left as such?  What
would happen if they were permitted?

 - The reason it is important to merge adjecent free blocks is because the
   allocation of blocks splits blocks to fit the requested size. If adjacent free blocks aren't merged together, the system will be unable to recognize that
   it can store a single larger chunk of data across the adjacent free blocks.


2) Which function(s) need to be concerned about adjacent free blocks?

  - Only the myfree() function needs to concerned with them. If it always checks for adjecent free
    blocks and merge if they are found, the problem will be solved before any other functions
    have to interact with the memory.


3) Name one advantage of each strategy.

  - "Best fit" has the advantage very low wasted space, at the cost of being processor intensive.

    "Worst fit" causes much fewer gaps in memory, and is also processor intensive.

    "First fit" has the advantage of being very fast since memory is allocated as soon as
    somewhere it can fit is found. 

    "Next fit" has the same advantage as first fit, with the added benefit of load balancing across 
    the entire memory.

4) Run the stress test on all strategies, and look at the results (tests.out).
What is the significance of "Average largest free block"?  Which strategy
generally has the best performance in this metric?  Why do you think this is?

5) In the stress test results (see Question 4), what is the significance of
"Average number of small blocks"?  Which strategy generally has the best
performance in this metric?  Why do you think this is?

  - "Average number of small blocks" indicates the level of external fragmentation.
    Best and first produces the highest numbers, worst gives a significantly lower one, 
    and next produces the by far lowest. 

6) Eventually, the many mallocs and frees produces many small blocks scattered
across the memory pool.  There may be enough space to allocate a new block, but
not in one place.  It is possible to compact the memory, so all the free blocks
are moved to one large free block.  How would you implement this in the system
you have built?

 - You go through the memory with an algorithm that moves up data so there's no spaces,
   in between blocks, and one large empty zone at the end. 
   In practice you would have to only do this reformatting once in a while. If you were to this
   constantly each time a change occured, it would be way too resource intensive. 


7) If you did implement memory compaction, what changes would you need to make
in how such a system is invoked (i.e. from a user's perspective)?

  - Since reformatting the entire memory is so resource intensive, the best solution on how often
    to do it may simply be to have the user invoke the operation.

8) How would you use the system you have built to implement realloc?  (Brief
explanation; no code)

  - We could search the linkedlist whether there is a block that fits the new size requirements. If there is we deallocate the current block and 
    allocate the new block

9) Which function(s) need to know which strategy is being used?  Briefly explain
why this/these and not others.

  - InitMem and myMalloc are the only functions that needs to know the strategy. Initmem is where the memory is initialized and we declare what strategy to use.
    myMalloc uses the declared strategy to allocate the blocks so it needs to know the strategy inorder to place them according to it.

10) Give one advantage of implementing memory management using a linked list
over a bit array, where every bit tells whether its corresponding byte is
allocated.

test @Thomasbt @SuhailDTU
