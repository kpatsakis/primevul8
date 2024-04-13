deallocate(void *p)
{
    ffi_cif *ptr = p;
    if (ptr->arg_types) xfree(ptr->arg_types);
    xfree(ptr);
}