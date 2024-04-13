setwidth0(struct table *t, struct table_mode *mode)
{
    int w;
    int width = t->tabcontentssize;
    struct table_cell *cell = &t->cell;

    if (t->col < 0)
	return -1;
    if (t->tabwidth[t->col] < 0)
	return -1;
    check_row(t, t->row);
    if (t->linfo.prev_spaces > 0)
	width -= t->linfo.prev_spaces;
    w = table_colspan(t, t->row, t->col);
    if (w == 1) {
	if (t->tabwidth[t->col] < width)
	    t->tabwidth[t->col] = width;
    }
    else if (cell->icell >= 0) {
	if (cell->width[cell->icell] < width)
	    cell->width[cell->icell] = width;
    }
    return width;
}