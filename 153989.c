end_table(struct table *tbl)
{
    struct table_cell *cell = &tbl->cell;
    int i, rulewidth = table_rule_width(tbl);
    if (rulewidth > 1) {
	if (tbl->total_width > 0)
	    tbl->total_width = ceil_at_intervals(tbl->total_width, rulewidth);
	for (i = 0; i <= tbl->maxcol; i++) {
	    tbl->minimum_width[i] =
		ceil_at_intervals(tbl->minimum_width[i], rulewidth);
	    tbl->tabwidth[i] = ceil_at_intervals(tbl->tabwidth[i], rulewidth);
	    if (tbl->fixed_width[i] > 0)
		tbl->fixed_width[i] =
		    ceil_at_intervals(tbl->fixed_width[i], rulewidth);
	}
	for (i = 0; i <= cell->maxcell; i++) {
	    cell->minimum_width[i] =
		ceil_at_intervals(cell->minimum_width[i], rulewidth);
	    cell->width[i] = ceil_at_intervals(cell->width[i], rulewidth);
	    if (cell->fixed_width[i] > 0)
		cell->fixed_width[i] =
		    ceil_at_intervals(cell->fixed_width[i], rulewidth);
	}
    }
    tbl->sloppy_width = fixed_table_width(tbl);
    if (tbl->total_width > tbl->sloppy_width)
	tbl->sloppy_width = tbl->total_width;
}