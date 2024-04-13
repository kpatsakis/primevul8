set_table_matrix0(struct table *t, int maxwidth)
{
    int size = t->maxcol + 1;
    int i, j, k, bcol, ecol;
    int width;
    double w0, w1, w, s, b;
#ifdef __GNUC__
    double we[size];
    char expand[size];
#else				/* not __GNUC__ */
    double we[MAXCOL];
    char expand[MAXCOL];
#endif				/* not __GNUC__ */
    struct table_cell *cell = &t->cell;

    w0 = 0.;
    for (i = 0; i < size; i++) {
	we[i] = weight(t->tabwidth[i]);
	w0 += we[i];
    }
    if (w0 <= 0.)
	w0 = 1.;

    if (cell->necell == 0) {
	for (i = 0; i < size; i++) {
	    s = we[i] / w0;
	    b = sigma_td_nw((int)(s * maxwidth));
	    correct_table_matrix2(t, i, 1, s, b);
	}
	return;
    }

    bzero(expand, size);

    for (k = 0; k < cell->necell; k++) {
	j = cell->eindex[k];
	bcol = cell->col[j];
	ecol = bcol + cell->colspan[j];
	width = cell->width[j] - (cell->colspan[j] - 1) * t->cellspacing;
	w1 = 0.;
	for (i = bcol; i < ecol; i++) {
	    w1 += t->tabwidth[i] + 0.1;
	    expand[i]++;
	}
	for (i = bcol; i < ecol; i++) {
	    w = weight(width * (t->tabwidth[i] + 0.1) / w1);
	    if (w > we[i])
		we[i] = w;
	}
    }

    w0 = 0.;
    w1 = 0.;
    for (i = 0; i < size; i++) {
	w0 += we[i];
	if (expand[i] == 0)
	    w1 += we[i];
    }
    if (w0 <= 0.)
	w0 = 1.;

    for (k = 0; k < cell->necell; k++) {
	j = cell->eindex[k];
	bcol = cell->col[j];
	width = cell->width[j] - (cell->colspan[j] - 1) * t->cellspacing;
	w = weight(width);
	s = w / (w1 + w);
	b = sigma_td_nw((int)(s * maxwidth));
	correct_table_matrix4(t, bcol, cell->colspan[j], expand, s, b);
    }

    for (i = 0; i < size; i++) {
	if (expand[i] == 0) {
	    s = we[i] / max(w1, 1.);
	    b = sigma_td_nw((int)(s * maxwidth));
	}
	else {
	    s = we[i] / max(w0 - w1, 1.);
	    b = sigma_td_nw(maxwidth);
	}
	correct_table_matrix3(t, i, expand, s, b);
    }
}