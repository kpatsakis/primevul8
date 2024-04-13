check_minimum_width(struct table *t, short *tabwidth)
{
    int i;
    struct table_cell *cell = &t->cell;

    for (i = 0; i <= t->maxcol; i++) {
	if (tabwidth[i] < t->minimum_width[i])
	    tabwidth[i] = t->minimum_width[i];
    }

    check_cell_width(tabwidth, cell->minimum_width, cell->col, cell->colspan,
		     cell->maxcell, cell->index, t->cellspacing, 0);
}