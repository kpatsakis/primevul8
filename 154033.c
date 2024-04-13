get_table_width(struct table *t, short *orgwidth, short *cellwidth, int flag)
{
#ifdef __GNUC__
    short newwidth[t->maxcol + 1];
#else				/* not __GNUC__ */
    short newwidth[MAXCOL];
#endif				/* not __GNUC__ */
    int i;
    int swidth;
    struct table_cell *cell = &t->cell;
    int rulewidth = table_rule_width(t);

    for (i = 0; i <= t->maxcol; i++)
	newwidth[i] = max(orgwidth[i], 0);

    if (flag & CHECK_FIXED) {
#ifdef __GNUC__
	short ccellwidth[cell->maxcell + 1];
#else				/* not __GNUC__ */
	short ccellwidth[MAXCELL];
#endif				/* not __GNUC__ */
	for (i = 0; i <= t->maxcol; i++) {
	    if (newwidth[i] < t->fixed_width[i])
		newwidth[i] = t->fixed_width[i];
	}
	for (i = 0; i <= cell->maxcell; i++) {
	    ccellwidth[i] = cellwidth[i];
	    if (ccellwidth[i] < cell->fixed_width[i])
		ccellwidth[i] = cell->fixed_width[i];
	}
	check_cell_width(newwidth, ccellwidth, cell->col, cell->colspan,
			 cell->maxcell, cell->index, t->cellspacing, 0);
    }
    else {
	check_cell_width(newwidth, cellwidth, cell->col, cell->colspan,
			 cell->maxcell, cell->index, t->cellspacing, 0);
    }
    if (flag & CHECK_MINIMUM)
	check_minimum_width(t, newwidth);

    swidth = 0;
    for (i = 0; i <= t->maxcol; i++) {
	swidth += ceil_at_intervals(newwidth[i], rulewidth);
    }
    swidth += table_border_width(t);
    return swidth;
}