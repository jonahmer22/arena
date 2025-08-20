# Changelog

All notable changes to this project will be documented in this file.

This project adheres to [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and follows [Semantic Versioning](https://semver.org/) (0.x indicates initial development).

## [v1.0] - 2025-08-19
### Changed
- Initial Commit of arena.h and arena.c including basic functionality for the library uploaded to repo.
### Added
- Added both a global and "local" API allowing for use of multiple independant arenas simultaneously within the same program.
##### Global:
- `arenaInit()`: Initializes global arena with one empty BUFF_SIZE (1MB by default) block of memory.
- `arenaAlloc(size_t size)`: Drop in replacement for malloc() call, allocates to global arena
- `arenaReset()`: Resets the arena and frees all blocks of memory and wipes the origin block for reuse.
- `arenaAllocBuffsizeBlock()`: A special function that returns a BUFF_SIZE (1MB by default) block that is never to be used by the arena. Usefull for allocating blocks of memory for specific purposes that should not be mixed with other memory. 
- `arenaDestroy()`: Destroys the global arena and frees all memory associated with it.
##### Local:
- `arenaLocalInit(Arena *arena)`: Initializes a local arena with one empty BUFF_SIZE (1MB by default) block of memory.
- `arenaLocalAlloc(Arena *arena, size_t size)`: Drop in replacement for malloc() call, allocates to pointed to arena
- `arenaLocalReset(Arena *arena)`: Resets the pointed to arena and frees all blocks of memory and wipes the origin block for reuse.
- `arenaLocalAllocBuffsizeBlock(Arena *arena)`: A special function that returns a BUFF_SIZE (1MB by default) block that is never to be used by the arena. Usefull for allocating blocks of memory for specific purposes that should not be mixed with other memory. 
- `arenaLocalDestroy(Arena *arena)`: Destroys the pointed to arena and frees all memory associated with it.

---