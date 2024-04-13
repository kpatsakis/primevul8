partial_unref(partial_T *pt)
{
    if (pt != NULL)
    {
	if (--pt->pt_refcount <= 0)
	    partial_free(pt);

	// If the reference count goes down to one, the funcstack may be the
	// only reference and can be freed if no other partials reference it.
	else if (pt->pt_refcount == 1 && pt->pt_funcstack != NULL)
	    funcstack_check_refcount(pt->pt_funcstack);
    }
}