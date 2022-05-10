// Copyright Maciej Polanski
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
    void* a = mmap(NULL, size, PROT_EXEC | PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, /*not a file mapping*/-1, 0);
    err(a, 1);
    // Grow 'a' to double size, also poof that grow is working
    a = mremap(a, size, 2 * size, MREMAP_MAYMOVE);
    err(a, 2);

    void* b = mmap(NULL, size, PROT_EXEC | PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, /*not a file mapping*/-1, 0);
    err(b, 3);    
    // Let's overwrite 'b' as second half of 'a'
    b = mremap(b, size, size, MREMAP_MAYMOVE | MREMAP_FIXED, a + size);
    err(b, 4);
    // And grow 'a' again. It fails, but only with overwrite above^
    a = mremap(a, 2 * size, 3 * size, MREMAP_MAYMOVE);
    err(a, 5);

    return 0;
}

