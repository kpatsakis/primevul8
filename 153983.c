check_compressible_cell(struct table *t, MAT * minv,
			double *newwidth, double *swidth, short *cwidth,
			double totalwidth, double *Sxx,
			int icol, int icell, double sxx, int corr)
{
    struct table_cell *cell = &t->cell;
    int i, j, k, m, bcol, ecol, span;
    double delta, owidth;
    double dmax, dmin, sxy;
    int rulewidth = table_rule_width(t);

    if (sxx < 10.)
	return corr;

    if (icol >= 0) {
	owidth = newwidth[icol];
	delta = newwidth[icol] - (double)t->tabwidth[icol];
	bcol = icol;
	ecol = bcol + 1;
    }
    else if (icell >= 0) {
	owidth = swidth[icell];
	delta = swidth[icell] - (double)cwidth[icell];
	bcol = cell->col[icell];
	ecol = bcol + cell->colspan[icell];
    }
    else {
	owidth = totalwidth;
	delta = totalwidth;
	bcol = 0;
	ecol = t->maxcol + 1;
    }

    dmin = delta;
    dmax = -1.;
    for (k = 0; k <= cell->maxcell; k++) {
	int bcol1, ecol1;
	int is_inclusive = 0;
	if (dmin <= 0.)
	    goto _end;
	j = cell->index[k];
	if (j == icell)
	    continue;
	bcol1 = cell->col[j];
	ecol1 = bcol1 + cell->colspan[j];
	sxy = 0.;
	for (m = bcol1; m < ecol1; m++) {
	    for (i = bcol; i < ecol; i++)
		sxy += m_entry(minv, i, m);
	}
	if (bcol1 >= bcol && ecol1 <= ecol) {
	    is_inclusive = 1;
	}
	if (sxy > 0.)
	    dmin = recalc_width(dmin, swidth[j], cwidth[j],
				sxx, Sxx[j], sxy, is_inclusive);
	else
	    dmax = recalc_width(dmax, swidth[j], cwidth[j],
				sxx, Sxx[j], sxy, is_inclusive);
    }
    for (m = 0; m <= t->maxcol; m++) {
	int is_inclusive = 0;
	if (dmin <= 0.)
	    goto _end;
	if (m == icol)
	    continue;
	sxy = 0.;
	for (i = bcol; i < ecol; i++)
	    sxy += m_entry(minv, i, m);
	if (m >= bcol && m < ecol) {
	    is_inclusive = 1;
	}
	if (sxy > 0.)
	    dmin = recalc_width(dmin, newwidth[m], t->tabwidth[m],
				sxx, m_entry(minv, m, m), sxy, is_inclusive);
	else
	    dmax = recalc_width(dmax, newwidth[m], t->tabwidth[m],
				sxx, m_entry(minv, m, m), sxy, is_inclusive);
    }
  _end:
    if (dmax > 0. && dmin > dmax)
	dmin = dmax;
    span = ecol - bcol;
    if ((span == t->maxcol + 1 && dmin >= 0.) ||
	(span != t->maxcol + 1 && dmin > rulewidth * 0.5)) {
	int nwidth = ceil_at_intervals(round(owidth - dmin), rulewidth);
	correct_table_matrix(t, bcol, ecol - bcol, nwidth, 1.);
	corr++;
    }
    return corr;
}