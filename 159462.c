nv_next(cmdarg_T *cap)
{
    pos_T   old = curwin->w_cursor;
    int	    wrapped = FALSE;
    int	    i = normal_search(cap, 0, NULL, SEARCH_MARK | cap->arg, &wrapped);

    if (i == 1 && !wrapped && EQUAL_POS(old, curwin->w_cursor))
    {
	// Avoid getting stuck on the current cursor position, which can
	// happen when an offset is given and the cursor is on the last char
	// in the buffer: Repeat with count + 1.
	cap->count1 += 1;
	(void)normal_search(cap, 0, NULL, SEARCH_MARK | cap->arg, NULL);
	cap->count1 -= 1;
    }
}