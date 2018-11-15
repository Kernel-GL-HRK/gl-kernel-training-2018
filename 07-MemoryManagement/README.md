# Memory Management

## Homework

1. Update uppercase converter from the [lesson #05 (Interfaces)](/05-Interfaces) to dynamically allocate memory for each writing  using SLAB allocator.
2. Update the module to preserve all previously written values:
    * Consecuent readings without writing should return converted values in a row starting from the last one.
3. Transform the module to use memory pool for allocation.
4. Implement dynamic changing of buffer size:
    * Set it to the size of the first written value (aligned to the nearest power of 2);
    * When written value exeeds element size - create new pool and move all existent elements to it.
