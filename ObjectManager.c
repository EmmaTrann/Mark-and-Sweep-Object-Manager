//-----------------------------------------
// NAME: Emma Tran
// STUDENT NUMBER:7876069
// COURSE: COMP 2160, SECTION: A01
// INSTRUCTOR: Saulo Santos
// ASSIGNMENT: assignment 4
//
// REMARKS:  Purpose of this program is to
// implement garbage collection
//-----------------------------------------
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ObjectManager.h"

// You can add other header files if needed

// tracks the next reference (ID) to use, we start at 1 so we can use 0 as the NULL reference
static Ref nextRef = 1;

// A Memblock holds the relevent information associated with an allocated block of memory by our memory manager
typedef struct MEMBLOCK MemBlock;

//define the bool type to use to mark a block in compact mark and sweep
typedef enum {false, true} bool;

// information needed to track our objects in memory
struct MEMBLOCK
{
    int numBytes;    // how big is this object?
    int startAddr;   // where the object starts
    Ref ref;         // the reference used to identify the object
    int count;       // the number of references to this object
    MemBlock *next;  // pointer to next block.  Blocks stored in a linked list.
    bool marked; // a bool to mark a block in compact mark and sweep
};

//prototype for compact since it is not in the header
void compact();

// The blocks are stored in a linked list where the start of the list is pointed to by memBlockStart.
static MemBlock *memBlockStart; // start of linked list of blocks allocated
static MemBlock *memBlockEnd;   // end of linked list of blocks allocated
static int numBlocks;            // number of blocks allocated

// our buffers.  This is where we allocate memory from.  One of these is always the current buffer.  The other is used for swapping
//during compaction stage.

static unsigned char buffer1[MEMORY_SIZE];
static unsigned char buffer2[MEMORY_SIZE];

// points to the current buffer containing our data
static unsigned char *currBuffer = buffer1;

// points to the location of the next available memory location
static int freeIndex = 0;


static void validateState() {
#ifndef NDEBUG
    // loop through linked list to check if each block has valid pointer or not
    for (MemBlock *currBlock = memBlockStart; currBlock != NULL; currBlock = currBlock->next) {
        assert(currBlock->startAddr >= 0 && currBlock->startAddr < MEMORY_SIZE);
    }
#endif

    // Make sure that freeIndex is within bounds
    assert(freeIndex >= 0 && freeIndex <= MEMORY_SIZE);
}

// performs required setup
void initPool()
{
    //write your code here
    memBlockStart = NULL;
    memBlockEnd = NULL;
    numBlocks = 0;
    //post condition
    assert(memBlockStart == NULL);
    assert(memBlockEnd == NULL);
    assert(numBlocks == 0);
}

// performs required clean up
void destroyPool()
{
    //we want to delete all nodes from the linked list.
    //write your code here
    //loop through the linked list of memblock to free/delete every single nodes
    MemBlock *curr;
    MemBlock *next;
    curr = memBlockStart;
    while (curr != NULL){
        next = curr->next;
        free(curr);
        curr = next;
    }

    //put everything back to initial state
    memBlockStart = NULL;
    memBlockEnd = NULL;
    numBlocks = 0;

    //post-condition: everything has been destroyed successfully
    assert(memBlockStart == NULL);
    assert(memBlockEnd == NULL);
    assert(numBlocks == 0);
}

// Adds the given object into our buffer. It will fire the garbage collector as required.
// We always assume that an insert always creates a new object...
// On success it returns the reference number for the block allocated.
// On failure it returns NULL_REF (0)
Ref insertObject( const int size )
{
    //write your code here
    Ref new_ref = NULL_REF;
    validateState();
    assert( size > 0 );
    //check to see if we have enough space/memory to insert the object
    //if not call compact to clean up first
    if ( size >= (MEMORY_SIZE-freeIndex) )
        compact();

    // if we do then perform to insert the object
    if ( size < (MEMORY_SIZE-freeIndex) ){
        //allocate memory for the new object
        MemBlock *newBlock = (MemBlock *)malloc(sizeof(MemBlock));
        assert (newBlock != NULL);
        if (newBlock != NULL){
            new_ref = nextRef++;

            //pre-condition
            assert( size > 0 );
            assert( new_ref >= 0 && new_ref < nextRef );
            assert( freeIndex >= 0 && freeIndex < MEMORY_SIZE );

            //create and initialize the new memblock
            newBlock->numBytes = size;
            newBlock->startAddr = freeIndex;
            newBlock->ref = new_ref;
            newBlock->next = NULL;
            newBlock->count=1;
            numBlocks++;

            //If there is already blocks
            //insert the object (i.e new memBlock) to the end
            if (memBlockEnd != NULL)
                memBlockEnd->next = newBlock;
            memBlockEnd = newBlock;

            //If this is the first block
            if (numBlocks == 1)
                memBlockStart = newBlock;

            // free the space in the memory
            memset( &currBuffer[freeIndex], 0x00, size );
            freeIndex += size;
            assert(freeIndex <= MEMORY_SIZE);
        }
    }
    //returns the reference number
    validateState();
    return new_ref;
}

//helper method
static MemBlock *find( const Ref ref ){
    MemBlock *index = NULL;
    MemBlock *ptr;
    ptr = memBlockStart;
    //loop through the linked list to search for the block that matched the ref
    while ((ptr != NULL)&& (index == NULL)){
        if (ptr->ref == ref){
            index = ptr;
        } else{
            ptr=ptr->next;
        }
    }
    return index;
}

// returns a pointer to the object being requested
void *retrieveObject( const Ref ref )
{
    //write your code here
    MemBlock *index;
    void *object;
    assert( ref >= 0 && ref < nextRef && "Invalid reference!" );
    index = find(ref);
    if (index != NULL){
        object = &currBuffer[index->startAddr];
    }
    //if not fail gracefully and print out the error msg
    else{
        printf("Cannot retrieve object.\n");
        exit(EXIT_FAILURE);
        object = NULL;
    }
    return object;
}

// update our index to indicate that we have another reference to the given object
void addReference( const Ref ref )
{
    //write your code here
    MemBlock *index;
    assert(ref >= 0 && ref < nextRef && "Invalid reference!");
    index = find(ref);
    if (index != NULL){
        index->count = index->count + 1;
    }
}

void dropReference( const Ref ref )
{
    //write your code here
    MemBlock *index;
    MemBlock *prev = NULL;
    MemBlock *curr = NULL;
    assert( ref >= 0 && ref < nextRef && "Invalid reference!");
    index = find(ref);
    if (index != NULL){
        index->count = index->count - 1;
        if (index->count == 0){
            //drop this block from the list if the count < 0
            if (index == memBlockStart){
                memBlockStart = memBlockStart->next;
                free(index);
            }else{
                prev = memBlockStart;
                curr = memBlockStart->next;
                while (curr != index){
                    prev = curr;
                    curr = curr->next;
                }
            }
            if (curr == memBlockEnd)
                memBlockEnd= prev;
            prev->next = index->next;
            free(index);
        }
        numBlocks--;
        if (numBlocks <= 1)
            memBlockEnd = memBlockStart;
    }
}

void compact()
{
    //write your code here
    //Mark phase
    printf("Calling compact function!\n");
    int count =0;
    int byte_count = freeIndex;
    for (MemBlock *block = memBlockStart; block != NULL; block = block->next) {
        block->marked = false;
        block->count++;
    }
    // Mark all the available blocks starting from the roots
    for (MemBlock *block = memBlockStart; block != NULL; block = block->next) {
        if (block->count > 0) {
            block->marked = true;
        }
    }

    // Step 2: Sweep phase
    // loop through the linked list and free all the unmarked blocks
    MemBlock **prev = &memBlockStart;
    MemBlock *block = memBlockStart;
    freeIndex =0;
    while (block != NULL) {
        if (!block->marked) {
            // free the block that is unmarked
            numBlocks--;
            *prev = block->next;
            if (block == memBlockEnd) {
                memBlockEnd = *prev;
            }
            free(block);
            block = *prev;
        } else {
            // The block is marked, move it to the buffer 2
            memcpy(&buffer2[freeIndex], &currBuffer[(*block).startAddr],(*block).numBytes );
            block->startAddr = freeIndex;
            freeIndex += block->numBytes;
            assert(freeIndex <= MEMORY_SIZE);
            prev = &block->next;
            block = block->next;
            count++;
        }
    }
    printf("The memory has been freed: %d bytes\n\n", (byte_count-freeIndex));

    //update buffer2 to be the current buffer
    currBuffer = buffer2;
    assert(currBuffer != NULL);
    assert(freeIndex >= 0);
    assert(freeIndex <= MEMORY_SIZE);
}

void dumpPool()
{
    //write your code here
    MemBlock *curr;
    curr = memBlockStart;
    while (curr != NULL){
        printf( "ref id = %lu, start address = %d, size = %d, count = %d\n", curr->ref, curr->startAddr, curr->numBytes,curr->count);
        curr=curr->next;
    }
    printf("Next free index is: %d\n", freeIndex);
}



