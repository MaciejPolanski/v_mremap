# _mremap_test.c_

This example uses non-POSIX `mremap()` call to connect chunks of anonymous memory allocated with `mmap()` into a continues region. I expected this to work based on documantation avaliable. However `mremap()` operation 'grow' unexpectadly fails, most likley due to in-kernel representation (known as VM/VMA) of memory chunk.

Alghorithm is as follows:
- Having memory A and B, grow A to (A+B) size _(possible move of A)_
- Move B to the newly added space of A
- Having also memory C, grow A to (A + B + C) size _(This step fails: `EFAULT 14 Bad address`)_
- Move C to newly added part of A

As 'grow' call works for a first time but second fails, I conclude that problem is that in second call memory is constructed from two separete in-kernel regions and `mremap` cannot handle such parameters.

Such diagnosis may have some support in the kernel comments, however I am not sure how to interpret `mm/mremap.c: mremap_to()`:
``` 
        /* We can't remap across vm area boundaries */
        if (old_len > vma->vm_end - addr)
                return ERR_PTR(-EFAULT);
```
On the other hand, there is no such requirement in the documentation, and it would be surprising to make the user space call dependent on the kernel representation. 

We may also read in documentation https://man7.org/linux/man-pages/man2/mremap.2.html:
```
EFAULT Some address in the range old_address to
              old_address+old_size is an invalid virtual memory address
              for this process.  You can also get EFAULT even if there
              exist mappings that cover the whole address space
              requested, but those mappings are of different types.
```
Usage of plural **'mappings'** explicitly suggests that it should be possible to use an area made up of multiple mappings.
