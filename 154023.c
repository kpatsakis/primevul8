check_relative_width(struct table *t, int maxwidth)
{
    int i;
    double rel_total = 0;
    int size = t->maxcol + 1;
    double *rcolwidth = New_N(double, size);
    struct table_cell *cell = &t->cell;
    int n_leftcol = 0;

    for (i = 0; i < size; i++)
	rcolwidth[i] = 0;

    for (i = 0; i < size; i++) {
	if (t->fixed_width[i] < 0)
	    rcolwidth[i] = -(double)t->fixed_width[i] / 100.0;
	else if (t->fixed_width[i] > 0)
	    rcolwidth[i] = (double)t->fixed_width[i] / maxwidth;
	else
	    n_leftcol++;
    }
    for (i = 0; i <= cell->maxcell; i++) {
	if (cell->fixed_width[i] < 0) {
	    double w = -(double)cell->fixed_width[i] / 100.0;
	    double r;
	    int j, k;
	    int n_leftcell = 0;
	    k = cell->col[i];
	    r = 0.0;
	    for (j = 0; j < cell->colspan[i]; j++) {
		if (rcolwidth[j + k] > 0)
		    r += rcolwidth[j + k];
		else
		    n_leftcell++;
	    }
	    if (n_leftcell == 0) {
		/* w must be identical to r */
		if (w != r)
		    cell->fixed_width[i] = -100 * r;
	    }
	    else {
		if (w <= r) {
		    /* make room for the left(width-unspecified) cell */
		    /* the next formula is an estimation of required width */
		    w = r * cell->colspan[i] / (cell->colspan[i] - n_leftcell);
		    cell->fixed_width[i] = -100 * w;
		}
		for (j = 0; j < cell->colspan[i]; j++) {
		    if (rcolwidth[j + k] == 0)
			rcolwidth[j + k] = (w - r) / n_leftcell;
		}
	    }
	}
	else if (cell->fixed_width[i] > 0) {
	    /* todo */
	}
    }
    /* sanity check */
    for (i = 0; i < size; i++)
	rel_total += rcolwidth[i];

    if ((n_leftcol == 0 && rel_total < 0.9) || 1.1 < rel_total) {
	for (i = 0; i < size; i++) {
	    rcolwidth[i] /= rel_total;
	}
	for (i = 0; i < size; i++) {
	    if (t->fixed_width[i] < 0)
		t->fixed_width[i] = -rcolwidth[i] * 100;
	}
	for (i = 0; i <= cell->maxcell; i++) {
	    if (cell->fixed_width[i] < 0) {
		double r;
		int j, k;
		k = cell->col[i];
		r = 0.0;
		for (j = 0; j < cell->colspan[i]; j++)
		    r += rcolwidth[j + k];
		cell->fixed_width[i] = -r * 100;
	    }
	}
    }
}