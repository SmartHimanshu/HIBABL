/* This file is responsible for making a custom memory allocator */

/*
It's implementation is a simple linked list.
We map 4MiB bytes initially and then we continue to 
give malloc next 1MiB if we run out of space.
*/

