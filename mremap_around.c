// Copyleft Maciej Polanski
#define _GNU_SOURCE 
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int size = 4096;

void err(void* e, int code)
{
    if (e == MAP_FAILED) {
        perror("mremap_test");
        exit(code);
    }
}

int main()
{
    // This test attaches new mapped memory to 'a' chunk.
    // It uses a workaround with alocating a new addres space each time.
    // First, let's create initial mmapped region 'a'
    void* a = mmap(NULL, size, PROT_EXEC | PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, /*not a file mapping*/-1, 0);
    err(a, 1);

    // Creation of 'b' and attaching it at the end of 'a'
    // 'a' need to be enlarged first, relocation is possible
    void* b = mmap(NULL, size, PROT_EXEC | PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, /*not a file mapping*/-1, 0);
    err(b, 2);    
    // Generate new bigger addres space for 'a', called 'a2'
    void* a2 = mmap(NULL, 2 * size, PROT_EXEC | PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE, /*not a file mapping*/-1, 0);
    err(a2, 3);
    // Move 'a' to new space
    a = mremap(a, size, size, MREMAP_MAYMOVE | MREMAP_FIXED, a2);
    err(a, 4);
    // Move 'b' to new space
    b = mremap(b, size, size, MREMAP_MAYMOVE | MREMAP_FIXED, a2 + size);
    err(b, 5);

    // Creation of 'c' and attaching it at the end of 'a', that is 'a2' now
    // 'a2' need to be enlarged first, relocation is possible
    void* c = mmap(NULL, size, PROT_EXEC | PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, /*not a file mapping*/-1, 0);
    err(c, 6);    
    // Generate new bigger addres space for 'a', called 'a3'
    void* a3 = mmap(NULL, 3 * size, PROT_EXEC | PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE, /*not a file mapping*/-1, 0);
    err(a3, 7);
    // Move 'a2', that is (2 * size), to new space
    a2 = mremap(a2, 2 * size, 2 * size, MREMAP_MAYMOVE | MREMAP_FIXED, a3);
    err(a2, 8);
    // Move 'b' to new space
    c = mremap(c, size, size, MREMAP_MAYMOVE | MREMAP_FIXED, a3 + 2 * size);
    err(c, 8);

    return 0;
}

