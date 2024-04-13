pushTable(struct table *tbl, struct table *tbl1)
{
    int col;
    int row;

    col = tbl->col;
    row = tbl->row;

    if (tbl->ntable >= tbl->tables_size) {
	struct table_in *tmp;
	tbl->tables_size += MAX_TABLE_N;
	tmp = New_N(struct table_in, tbl->tables_size);
	if (tbl->tables)
	    bcopy(tbl->tables, tmp, tbl->ntable * sizeof(struct table_in));
	tbl->tables = tmp;
    }

    tbl->tables[tbl->ntable].ptr = tbl1;
    tbl->tables[tbl->ntable].col = col;
    tbl->tables[tbl->ntable].row = row;
    tbl->tables[tbl->ntable].indent = tbl->indent;
    tbl->tables[tbl->ntable].buf = newTextLineList();
    check_row(tbl, row);
    if (col + 1 <= tbl->maxcol && tbl->tabattr[row][col + 1] & HTT_X)
	tbl->tables[tbl->ntable].cell = tbl->cell.icell;
    else
	tbl->tables[tbl->ntable].cell = -1;
    tbl->ntable++;
}