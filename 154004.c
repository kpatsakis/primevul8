set_integered_width(struct table *t, double *dwidth, short *iwidth)
{
    int i, j, k, n, bcol, ecol, step;
    short *indexarray;
    char *fixed;
    double *mod;
    double sum = 0., x = 0.;
    struct table_cell *cell = &t->cell;
    int rulewidth = table_rule_width(t);

    indexarray = NewAtom_N(short, t->maxcol + 1);
    mod = NewAtom_N(double, t->maxcol + 1);
    for (i = 0; i <= t->maxcol; i++) {
	iwidth[i] = ceil_at_intervals(ceil(dwidth[i]), rulewidth);
	mod[i] = (double)iwidth[i] - dwidth[i];
    }

    sum = 0.;
    for (k = 0; k <= t->maxcol; k++) {
	x = mod[k];
	sum += x;
	i = bsearch_double(x, mod, indexarray, k);
	if (k > i) {
	    int ii;
	    for (ii = k; ii > i; ii--)
		indexarray[ii] = indexarray[ii - 1];
	}
	indexarray[i] = k;
    }

    fixed = NewAtom_N(char, t->maxcol + 1);
    bzero(fixed, t->maxcol + 1);
    for (step = 0; step < 2; step++) {
	for (i = 0; i <= t->maxcol; i += n) {
	    int nn;
	    char *idx;
	    double nsum;
	    if (sum < 0.5)
		return;
	    for (n = 0; i + n <= t->maxcol; n++) {
		int ii = indexarray[i + n];
		if (n == 0)
		    x = mod[ii];
		else if (fabs(mod[ii] - x) > 1e-6)
		    break;
	    }
	    for (k = 0; k < n; k++) {
		int ii = indexarray[i + k];
		if (fixed[ii] < 2 &&
		    iwidth[ii] - rulewidth < t->minimum_width[ii])
		    fixed[ii] = 2;
		if (fixed[ii] < 1 &&
		    iwidth[ii] - rulewidth < t->tabwidth[ii] &&
		    (double)rulewidth - mod[ii] > 0.5)
		    fixed[ii] = 1;
	    }
	    idx = NewAtom_N(char, n);
	    for (k = 0; k < cell->maxcell; k++) {
		int kk, w, width, m;
		j = cell->index[k];
		bcol = cell->col[j];
		ecol = bcol + cell->colspan[j];
		m = 0;
		for (kk = 0; kk < n; kk++) {
		    int ii = indexarray[i + kk];
		    if (ii >= bcol && ii < ecol) {
			idx[m] = ii;
			m++;
		    }
		}
		if (m == 0)
		    continue;
		width = (cell->colspan[j] - 1) * t->cellspacing;
		for (kk = bcol; kk < ecol; kk++)
		    width += iwidth[kk];
		w = 0;
		for (kk = 0; kk < m; kk++) {
		    if (fixed[(int)idx[kk]] < 2)
			w += rulewidth;
		}
		if (width - w < cell->minimum_width[j]) {
		    for (kk = 0; kk < m; kk++) {
			if (fixed[(int)idx[kk]] < 2)
			    fixed[(int)idx[kk]] = 2;
		    }
		}
		w = 0;
		for (kk = 0; kk < m; kk++) {
		    if (fixed[(int)idx[kk]] < 1 &&
			(double)rulewidth - mod[(int)idx[kk]] > 0.5)
			w += rulewidth;
		}
		if (width - w < cell->width[j]) {
		    for (kk = 0; kk < m; kk++) {
			if (fixed[(int)idx[kk]] < 1 &&
			    (double)rulewidth - mod[(int)idx[kk]] > 0.5)
			    fixed[(int)idx[kk]] = 1;
		    }
		}
	    }
	    nn = 0;
	    for (k = 0; k < n; k++) {
		int ii = indexarray[i + k];
		if (fixed[ii] <= step)
		    nn++;
	    }
	    nsum = sum - (double)(nn * rulewidth);
	    if (nsum < 0. && fabs(sum) <= fabs(nsum))
		return;
	    for (k = 0; k < n; k++) {
		int ii = indexarray[i + k];
		if (fixed[ii] <= step) {
		    iwidth[ii] -= rulewidth;
		    fixed[ii] = 3;
		}
	    }
	    sum = nsum;
	}
    }
}