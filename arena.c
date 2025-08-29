#include "arena.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

// =======
// private
// =======

// Global arena
static Arena* arena;

// Initializes an empty MemBlock of size `aligned_alloc` to align and returns a pointer to it
static inline MemBlock* memBlockInit(size_t size, size_t align){
	MemBlock *memBlock = malloc(sizeof(MemBlock));
	if(!memBlock){
		memBlock = malloc(sizeof(MemBlock));
		if(!memBlock){
			perror("[FATAL]: Could not allocate MemBlock.");

			exit(10);
		}
	}

	size = ROUND_UP(size, align);
	memBlock->buffer = aligned_alloc(align, size);
	if(!memBlock->buffer){
		memBlock->buffer = aligned_alloc(align, size);
		if(!memBlock->buffer){
			perror("[FATAL]: Could not allocate MemBlock buffer.");
			free(memBlock);

			exit(11);	
		}
	}

	memBlock->nextBlock = NULL;
	memBlock->head = 0;
	memBlock->size = size;

	return memBlock;
}

// Adds a MemBlock to the linked list of MemBlocks 
static inline MemBlock* memBlockAdd(MemBlock *memBlock, size_t size){
	memBlock->nextBlock = memBlockInit(size, ARENA_ALIGN);

	return memBlock->nextBlock;
}

// Adds a MemBlock to the linked list of MemBlocks aligned to the size of BuffSize
static inline MemBlock* memBlockAddAlignedBuff(MemBlock *memBlock){
	memBlock->nextBlock = memBlockInit(BUFF_SIZE, BUFF_SIZE);

	return memBlock->nextBlock;
}

// Destroys and frees a MemBlock returning a pointer to the next one in the list
static inline MemBlock* memBlockDestroy(MemBlock* memBlock){
	MemBlock *temp = memBlock->nextBlock;
	
	free(memBlock->buffer);
	memBlock->buffer = NULL;

	memBlock->size = 0;
	memBlock->head = 0;

	memBlock->nextBlock = NULL;

	free(memBlock);
	
	return temp;
}

// =====
// local
// =====

// Initializes a Arena with an empty BUFF_SIZE MemBlock
// returns a pointer to the Arena
inline Arena* arenaLocalInit(){
	Arena *larena = malloc(sizeof(Arena));
	if(!larena){
		larena = malloc(sizeof(Arena));
		if(!larena){
			perror("[FATAL]: Could not allocate Arena.");

			exit(12);
		}
	}
	
	larena->numBlocks = 1;
	larena->head = memBlockInit(BUFF_SIZE, ARENA_ALIGN);
	larena->tail = larena->head;

	return larena;
}

// Destroys and frees all associated memory with a Arena passed in as an arguement
inline void arenaLocalDestroy(Arena *larena){
	if(!larena){
		perror("[FATAL]: Cannot destroy uninitialized arena.");

		exit(13);
	}

	MemBlock* temp = larena->head;
	for(; temp != NULL;){
		temp = memBlockDestroy(temp);
	}

	larena->head = NULL;
	larena->tail = NULL;

	larena->numBlocks = 0;

	free(larena);
	
	return;
}

// Frees all memory associated with arena passed as arguement except the original BUFF_SIZE MemBlock and resets all heads to the base of the block
// returns a pointer to the Arena passed as arguement, which is optional to use
inline Arena* arenaLocalReset(Arena *larena){
	if(!larena){
		perror("[FATAL]: Cannot reset uninitialized arena.");

		exit(14);
	}
	
	MemBlock* temp = larena->head->nextBlock;
	for(; temp != NULL;){
		temp = memBlockDestroy(temp);
	}

	arena->head->nextBlock = NULL;
	arena->tail = arena->head;
	arena->head = 0;

	memset(larena->head->buffer, 0, BUFF_SIZE);

	return larena;
}

// Returns a pointer to the base of a block of memory numBytes in size
// increments all Arena head pointers and allocates extra memory if needed to the Arena passed as an arguement
inline void* arenaLocalAlloc(Arena *larena, size_t numBytes){
	if(!larena){
		perror("[FATAL]: Cannot allocate to uninitialized arena.");

		exit(15);
	}

	numBytes = ROUND_UP(numBytes, ARENA_ALIGN);

	if(numBytes > BUFF_SIZE){
		// add a block specifically for this big chunk of data
		larena->tail = memBlockAdd(larena->tail, numBytes);

		void* ptr = larena->tail->buffer + larena->tail->head;
		larena->tail->head += numBytes;

		return ptr;
	}
	else if(larena->tail->head + numBytes > larena->tail->size){
		// add on a new block and just add to there
		larena->tail = memBlockAdd(larena->tail, BUFF_SIZE);
        
		void* ptr = larena->tail->buffer + larena->tail->head;
		larena->tail->head += numBytes;

		return ptr;
	}
	else{
        	// if we can just fit it in our current block
		void* ptr = larena->tail->buffer + larena->tail->head;
		larena->tail->head += numBytes;

		return ptr;
	}
}

// Allocates a BUFF_SIZE MemBlock
// a pointer is returned to the base of the MemBlock and it is marked as full to the Arena passed as an arguement
inline void* arenaLocalAllocBuffsizeBlock(Arena *larena){
	if(!larena){
		perror("[FATAL]: Cannot allocate to uninitialized arena.");

		exit(16);
	}

	larena->tail = memBlockAddAlignedBuff(larena->tail);

	void* ptr = larena->tail->buffer + larena->tail->head;
	larena->tail->head = BUFF_SIZE;

	return ptr;
}

// ======
// global
// ======

// Initializes the global Arena with an empty BUFF_SIZE MemBlock
// returns a pointer to the global Arena, which is optional to use
inline Arena* arenaInit(){
	if(!arena){
		arena = arenaLocalInit();
	}

	return arena;
}

// Destroys and frees all associated memory with the global Arena
inline void arenaDestroy(){
	arenaLocalDestroy(arena);    
}

// Frees all memory in the global arena except the original BUFF_SIZE MemBlock and resets all heads to the base of the block
// returns a pointer to the global Arena, which is optional to use
inline Arena* arenaReset(){
	return arenaLocalReset(arena);
}

// Returns a pointer to the base of a block of memory numBytes in size
// increments all global Arena head pointers and allocates extra memory if needed
inline void* arenaAlloc(size_t numBytes){
	if(!arena){
		// because we know the arena instead of just printing an error and exiting we can create it for the user
		arenaInit();
	}

	return arenaLocalAlloc(arena, numBytes);    
}

// Allocates a BUFF_SIZE MemBlock
// a pointer is returned to the base of the MemBlock and it is marked as full to the Arena
inline void* arenaAllocBuffsizeBlock(){
	if(!arena){
		// can initialize the arena for the user as we know which one they intend to use
		arenaInit();
	}
	
	return arenaLocalAllocBuffsizeBlock(arena);
}

