tagstack_set_curidx(win_T *wp, int curidx)
{
    wp->w_tagstackidx = curidx;
    if (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    if (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}