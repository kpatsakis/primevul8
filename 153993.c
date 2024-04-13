suspend_or_pushdata(struct table *tbl, char *line)
{
    if (tbl->flag & TBL_IN_COL)
	pushdata(tbl, tbl->row, tbl->col, line);
    else {
	if (!tbl->suspended_data)
	    tbl->suspended_data = newTextList();
	pushText(tbl->suspended_data, line ? line : "");
    }
}