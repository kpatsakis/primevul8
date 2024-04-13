partial_free(partial_T *pt)
{
    int i;

    for (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    if (pt->pt_name != NULL)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    else
	func_ptr_unref(pt->pt_func);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.out_up_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    if (pt->pt_funcstack != NULL)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }

    vim_free(pt);
}