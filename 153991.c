get_spec_cell_width(struct table *tbl, int row, int col)
{
    int i, w;

    w = tbl->tabwidth[col];
    for (i = col + 1; i <= tbl->maxcol; i++) {
	check_row(tbl, row);
	if (tbl->tabattr[row][i] & HTT_X)
	    w += tbl->tabwidth[i] + tbl->cellspacing;
	else
	    break;
    }
    return w;
}