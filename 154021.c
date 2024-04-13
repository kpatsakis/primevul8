table_colspan(struct table *t, int row, int col)
{
    int i;
    for (i = col + 1; i <= t->maxcol && (t->tabattr[row][i] & HTT_X); i++) ;
    return i - col;
}