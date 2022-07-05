# _mremap_test.c_

This example uses non-POSIX `mremap()` call to connect chunks of anonymous memory allocated with `mmap()` into a one continues region. Based on the avaliable documantation I expected this to work properly. However `mremap()` operation 'grow' unexpectedly fails, most likley due to in-kernel representation (known as VM/VMA) of memory chunk, where one continous userspace region is still two separate VM/VMA inside a kernel.

Algorithm is as follows:
- Having memory A and B, grow A to (A+B) size _(move of A may happens)_
- Move B to a newly added space at the end of A
- Having also C memory, grow A to (A + B + C) size _(This step fails: `EFAULT 14 Bad address`)_
- Move C to a newly added space at the end of A

Since 'grow' call works the first time, but dosen't work at the second time, I conclude that problem in second call is that memory is constructed from two separete in-kernel regions so `mremap` cannot handle such parameter.

Such diagnosis may have some support in the kernel comments, however I am not sure how to interpret `mm/mremap.c: mremap_to()`:
``` 
        /* We can't remap across vm area boundaries */
        if (old_len > vma->vm_end - addr)
                return ERR_PTR(-EFAULT);
```
On the other hand, there is no such requirement in the documentation, and it would be surprising to make the user space call dependent on an internal kernel representation. 

We may also read in documentation https://man7.org/linux/man-pages/man2/mremap.2.html:
```
EFAULT Some address in the range old_address to
              old_address+old_size is an invalid virtual memory address
              for this process.  You can also get EFAULT even if there
              exist mappings that cover the whole address space
              requested, but those mappings are of different types.
```
Usage plural **'mappings'** clearly suggests that it should be possible to use an area composed of multiple mappings (provided types are the same).
