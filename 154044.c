check_maximum_width(struct table *t)
{
    struct table_cell *cell = &t->cell;
#ifdef MATRIX
    int i, j, bcol, ecol;
    int swidth, width;

    cell->necell = 0;
    for (j = 0; j <= cell->maxcell; j++) {
	bcol = cell->col[j];
	ecol = bcol + cell->colspan[j];
	swidth = 0;
	for (i = bcol; i < ecol; i++)
	    swidth += t->tabwidth[i];

	width = cell->width[j] - (cell->colspan[j] - 1) * t->cellspacing;
	if (width > swidth) {
	    cell->eindex[cell->necell] = j;
	    cell->necell++;
	}
    }
#else				/* not MATRIX */
    check_cell_width(t->tabwidth, cell->width, cell->col, cell->colspan,
		     cell->maxcell, cell->index, t->cellspacing, 0);
    check_minimum_width(t, t->tabwidth);
#endif				/* not MATRIX */
}