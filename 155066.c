frame_remove(frame_T *frp)
{
    if (frp->fr_prev != NULL)
	frp->fr_prev->fr_next = frp->fr_next;
    else
    {
	frp->fr_parent->fr_child = frp->fr_next;
	/* special case: topframe->fr_child == frp */
	if (topframe->fr_child == frp)
	    topframe->fr_child = frp->fr_next;
    }
    if (frp->fr_next != NULL)
	frp->fr_next->fr_prev = frp->fr_prev;
}