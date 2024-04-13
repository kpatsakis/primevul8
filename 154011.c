table_rowspan(struct table *t, int row, int col)
{
    int i;
    if (!t->tabattr[row])
	return 0;
    for (i = row + 1; i <= t->maxrow && t->tabattr[i] &&
	 (t->tabattr[i][col] & HTT_Y); i++) ;
    return i - row;
}