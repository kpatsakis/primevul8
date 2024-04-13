check_rowcol(struct table *tbl, struct table_mode *mode)
{
    int row = tbl->row, col = tbl->col;

    if (!(tbl->flag & TBL_IN_ROW)) {
	tbl->flag |= TBL_IN_ROW;
	tbl->row++;
	if (tbl->row > tbl->maxrow)
	    tbl->maxrow = tbl->row;
	tbl->col = -1;
    }
    if (tbl->row == -1)
	tbl->row = 0;
    if (tbl->col == -1)
	tbl->col = 0;

    for (;; tbl->row++) {
	check_row(tbl, tbl->row);
	for (; tbl->col < MAXCOL &&
	     tbl->tabattr[tbl->row][tbl->col] & (HTT_X | HTT_Y); tbl->col++) ;
	if (tbl->col < MAXCOL)
	    break;
	tbl->col = 0;
    }
    if (tbl->row > tbl->maxrow)
	tbl->maxrow = tbl->row;
    if (tbl->col > tbl->maxcol)
	tbl->maxcol = tbl->col;

    if (tbl->row != row || tbl->col != col)
	begin_cell(tbl, mode);
    tbl->flag |= TBL_IN_COL;
}