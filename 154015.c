set_table_width(struct table *t, short *newwidth, int maxwidth)
{
    int i, j, k, bcol, ecol;
    struct table_cell *cell = &t->cell;
    char *fixed;
    int swidth, fwidth, width, nvar;
    double s;
    double *dwidth;
    int try_again;

    fixed = NewAtom_N(char, t->maxcol + 1);
    bzero(fixed, t->maxcol + 1);
    dwidth = NewAtom_N(double, t->maxcol + 1);

    for (i = 0; i <= t->maxcol; i++) {
	dwidth[i] = 0.0;
	if (t->fixed_width[i] < 0) {
	    t->fixed_width[i] = -t->fixed_width[i] * maxwidth / 100;
	}
	if (t->fixed_width[i] > 0) {
	    newwidth[i] = t->fixed_width[i];
	    fixed[i] = 1;
	}
	else
	    newwidth[i] = 0;
	if (newwidth[i] < t->minimum_width[i])
	    newwidth[i] = t->minimum_width[i];
    }

    for (k = 0; k <= cell->maxcell; k++) {
	j = cell->indexarray[k];
	bcol = cell->col[j];
	ecol = bcol + cell->colspan[j];

	if (cell->fixed_width[j] < 0)
	    cell->fixed_width[j] = -cell->fixed_width[j] * maxwidth / 100;

	swidth = 0;
	fwidth = 0;
	nvar = 0;
	for (i = bcol; i < ecol; i++) {
	    if (fixed[i]) {
		fwidth += newwidth[i];
	    }
	    else {
		swidth += newwidth[i];
		nvar++;
	    }
	}
	width = max(cell->fixed_width[j], cell->minimum_width[j])
	    - (cell->colspan[j] - 1) * t->cellspacing;
	if (nvar > 0 && width > fwidth + swidth) {
	    s = 0.;
	    for (i = bcol; i < ecol; i++) {
		if (!fixed[i])
		    s += weight3(t->tabwidth[i]);
	    }
	    for (i = bcol; i < ecol; i++) {
		if (!fixed[i])
		    dwidth[i] = (width - fwidth) * weight3(t->tabwidth[i]) / s;
		else
		    dwidth[i] = (double)newwidth[i];
	    }
	    dv2sv(dwidth, newwidth, cell->colspan[j]);
	    if (cell->fixed_width[j] > 0) {
		for (i = bcol; i < ecol; i++)
		    fixed[i] = 1;
	    }
	}
    }

    do {
	nvar = 0;
	swidth = 0;
	fwidth = 0;
	for (i = 0; i <= t->maxcol; i++) {
	    if (fixed[i]) {
		fwidth += newwidth[i];
	    }
	    else {
		swidth += newwidth[i];
		nvar++;
	    }
	}
	width = maxwidth - t->maxcol * t->cellspacing;
	if (nvar == 0 || width <= fwidth + swidth)
	    break;

	s = 0.;
	for (i = 0; i <= t->maxcol; i++) {
	    if (!fixed[i])
		s += weight3(t->tabwidth[i]);
	}
	for (i = 0; i <= t->maxcol; i++) {
	    if (!fixed[i])
		dwidth[i] = (width - fwidth) * weight3(t->tabwidth[i]) / s;
	    else
		dwidth[i] = (double)newwidth[i];
	}
	dv2sv(dwidth, newwidth, t->maxcol + 1);

	try_again = 0;
	for (i = 0; i <= t->maxcol; i++) {
	    if (!fixed[i]) {
		if (newwidth[i] > t->tabwidth[i]) {
		    newwidth[i] = t->tabwidth[i];
		    fixed[i] = 1;
		    try_again = 1;
		}
		else if (newwidth[i] < t->minimum_width[i]) {
		    newwidth[i] = t->minimum_width[i];
		    fixed[i] = 1;
		    try_again = 1;
		}
	    }
	}
    } while (try_again);
}