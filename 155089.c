win_init(win_T *newp, win_T *oldp, int flags UNUSED)
{
    int		i;

    newp->w_buffer = oldp->w_buffer;
#ifdef FEAT_SYN_HL
    newp->w_s = &(oldp->w_buffer->b_s);
#endif
    oldp->w_buffer->b_nwindows++;
    newp->w_cursor = oldp->w_cursor;
    newp->w_valid = 0;
    newp->w_curswant = oldp->w_curswant;
    newp->w_set_curswant = oldp->w_set_curswant;
    newp->w_topline = oldp->w_topline;
#ifdef FEAT_DIFF
    newp->w_topfill = oldp->w_topfill;
#endif
    newp->w_leftcol = oldp->w_leftcol;
    newp->w_pcmark = oldp->w_pcmark;
    newp->w_prev_pcmark = oldp->w_prev_pcmark;
    newp->w_alt_fnum = oldp->w_alt_fnum;
    newp->w_wrow = oldp->w_wrow;
    newp->w_fraction = oldp->w_fraction;
    newp->w_prev_fraction_row = oldp->w_prev_fraction_row;
#ifdef FEAT_JUMPLIST
    copy_jumplist(oldp, newp);
#endif
#ifdef FEAT_QUICKFIX
    if (flags & WSP_NEWLOC)
    {
	/* Don't copy the location list.  */
	newp->w_llist = NULL;
	newp->w_llist_ref = NULL;
    }
    else
	copy_loclist_stack(oldp, newp);
#endif
    newp->w_localdir = (oldp->w_localdir == NULL)
				    ? NULL : vim_strsave(oldp->w_localdir);

    /* copy tagstack and folds */
    for (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*tag = &newp->w_tagstack[i];
	*tag = oldp->w_tagstack[i];
	if (tag->tagname != NULL)
	    tag->tagname = vim_strsave(tag->tagname);
	if (tag->user_data != NULL)
	    tag->user_data = vim_strsave(tag->user_data);
    }
    newp->w_tagstackidx = oldp->w_tagstackidx;
    newp->w_tagstacklen = oldp->w_tagstacklen;
#ifdef FEAT_FOLDING
    copyFoldingState(oldp, newp);
#endif

    win_init_some(newp, oldp);

#ifdef FEAT_SYN_HL
    check_colorcolumn(newp);
#endif
}