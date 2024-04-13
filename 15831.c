tagstack_shift(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    int		i;

    tagstack_clear_entry(&tagstack[0]);
    for (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}