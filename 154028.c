check_minimum0(struct table *t, int min)
{
    int i, w, ww;
    struct table_cell *cell;

    if (t->col < 0)
	return;
    if (t->tabwidth[t->col] < 0)
	return;
    check_row(t, t->row);
    w = table_colspan(t, t->row, t->col);
    min += t->indent;
    if (w == 1)
	ww = min;
    else {
	cell = &t->cell;
	ww = 0;
	if (cell->icell >= 0 && cell->minimum_width[cell->icell] < min)
	    cell->minimum_width[cell->icell] = min;
    }
    for (i = t->col;
	 i <= t->maxcol && (i == t->col || (t->tabattr[t->row][i] & HTT_X));
	 i++) {
	if (t->minimum_width[i] < ww)
	    t->minimum_width[i] = ww;
    }
}