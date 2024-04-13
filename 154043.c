check_table_width(struct table *t, double *newwidth, MAT * minv, int itr)
{
    int i, j, k, m, bcol, ecol;
    int corr = 0;
    struct table_cell *cell = &t->cell;
#ifdef __GNUC__
    short orgwidth[t->maxcol + 1], corwidth[t->maxcol + 1];
    short cwidth[cell->maxcell + 1];
    double swidth[cell->maxcell + 1];
#else				/* __GNUC__ */
    short orgwidth[MAXCOL], corwidth[MAXCOL];
    short cwidth[MAXCELL];
    double swidth[MAXCELL];
#endif				/* __GNUC__ */
    double twidth, sxy, *Sxx, stotal;

    twidth = 0.;
    stotal = 0.;
    for (i = 0; i <= t->maxcol; i++) {
	twidth += newwidth[i];
	stotal += m_entry(minv, i, i);
	for (m = 0; m < i; m++) {
	    stotal += 2 * m_entry(minv, i, m);
	}
    }

    Sxx = NewAtom_N(double, cell->maxcell + 1);
    for (k = 0; k <= cell->maxcell; k++) {
	j = cell->index[k];
	bcol = cell->col[j];
	ecol = bcol + cell->colspan[j];
	swidth[j] = 0.;
	for (i = bcol; i < ecol; i++)
	    swidth[j] += newwidth[i];
	cwidth[j] = cell->width[j] - (cell->colspan[j] - 1) * t->cellspacing;
	Sxx[j] = 0.;
	for (i = bcol; i < ecol; i++) {
	    Sxx[j] += m_entry(minv, i, i);
	    for (m = bcol; m <= ecol; m++) {
		if (m < i)
		    Sxx[j] += 2 * m_entry(minv, i, m);
	    }
	}
    }

    /* compress table */
    corr = check_compressible_cell(t, minv, newwidth, swidth,
				   cwidth, twidth, Sxx, -1, -1, stotal, corr);
    if (itr < MAX_ITERATION && corr > 0)
	return corr;

    /* compress multicolumn cell */
    for (k = cell->maxcell; k >= 0; k--) {
	j = cell->index[k];
	corr = check_compressible_cell(t, minv, newwidth, swidth,
				       cwidth, twidth, Sxx,
				       -1, j, Sxx[j], corr);
	if (itr < MAX_ITERATION && corr > 0)
	    return corr;
    }

    /* compress single column cell */
    for (i = 0; i <= t->maxcol; i++) {
	corr = check_compressible_cell(t, minv, newwidth, swidth,
				       cwidth, twidth, Sxx,
				       i, -1, m_entry(minv, i, i), corr);
	if (itr < MAX_ITERATION && corr > 0)
	    return corr;
    }


    for (i = 0; i <= t->maxcol; i++)
	corwidth[i] = orgwidth[i] = round(newwidth[i]);

    check_minimum_width(t, corwidth);

    for (i = 0; i <= t->maxcol; i++) {
	double sx = sqrt(m_entry(minv, i, i));
	if (sx < 0.1)
	    continue;
	if (orgwidth[i] < t->minimum_width[i] &&
	    corwidth[i] == t->minimum_width[i]) {
	    double w = (sx > 0.5) ? 0.5 : sx * 0.2;
	    sxy = 0.;
	    for (m = 0; m <= t->maxcol; m++) {
		if (m == i)
		    continue;
		sxy += m_entry(minv, i, m);
	    }
	    if (sxy <= 0.) {
		correct_table_matrix(t, i, 1, t->minimum_width[i], w);
		corr++;
	    }
	}
    }

    for (k = 0; k <= cell->maxcell; k++) {
	int nwidth = 0, mwidth;
	double sx;

	j = cell->index[k];
	sx = sqrt(Sxx[j]);
	if (sx < 0.1)
	    continue;
	bcol = cell->col[j];
	ecol = bcol + cell->colspan[j];
	for (i = bcol; i < ecol; i++)
	    nwidth += corwidth[i];
	mwidth =
	    cell->minimum_width[j] - (cell->colspan[j] - 1) * t->cellspacing;
	if (mwidth > swidth[j] && mwidth == nwidth) {
	    double w = (sx > 0.5) ? 0.5 : sx * 0.2;

	    sxy = 0.;
	    for (i = bcol; i < ecol; i++) {
		for (m = 0; m <= t->maxcol; m++) {
		    if (m >= bcol && m < ecol)
			continue;
		    sxy += m_entry(minv, i, m);
		}
	    }
	    if (sxy <= 0.) {
		correct_table_matrix(t, bcol, cell->colspan[j], mwidth, w);
		corr++;
	    }
	}
    }

    if (itr >= MAX_ITERATION)
	return 0;
    else
	return corr;
}