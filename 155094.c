win_init_some(win_T *newp, win_T *oldp)
{
    /* Use the same argument list. */
    newp->w_alist = oldp->w_alist;
    ++newp->w_alist->al_refcount;
    newp->w_arg_idx = oldp->w_arg_idx;

    /* copy options from existing window */
    win_copy_options(oldp, newp);
}