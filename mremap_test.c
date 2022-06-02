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
    // However, it fails at second time for unclear reason.
    // First, let's create initial mmapped region 'a'
    void* a = mmap(NULL, size, PROT_EXEC | PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, /*not a file mapping*/-1, 0);
    err(a, 1);

    // Creation of 'b' and attaching it at the end of 'a'
    // 'a' need to be enlarged first, relocation is possible
    void* b = mmap(NULL, size, PROT_EXEC | PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, /*not a file mapping*/-1, 0);
    err(b, 2);    
    a = mremap(a, size, 2 * size, MREMAP_MAYMOVE);                      // Grow 'a'
    err(a, 3);
    b = mremap(b, size, size, MREMAP_MAYMOVE | MREMAP_FIXED, a + size); // Attach 'b' at the end
    err(b, 4);

    // Creation of 'c' and attaching it at the end of 'a'
    // 'a' need to be enlarged first, relocation is possible
    void* c = mmap(NULL, size, PROT_EXEC | PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, /*not a file mapping*/-1, 0);
    err(c, 5);    
    a = mremap(a, 2 * size, 3 * size, MREMAP_MAYMOVE);                   // <- this fails: EFAULT 14 Bad address
    err(a, 6);                                                           // exit code 6
    c = mremap(c, size, size, MREMAP_MAYMOVE | MREMAP_FIXED, a + 2 * size); 
    err(b, 7);

    return 0;
}

