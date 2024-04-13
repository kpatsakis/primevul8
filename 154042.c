pushdata(struct table *t, int row, int col, char *data)
{
    check_row(t, row);
    if (t->tabdata[row][col] == NULL)
	t->tabdata[row][col] = newGeneralList();

    pushText(t->tabdata[row][col], data ? data : "");
}