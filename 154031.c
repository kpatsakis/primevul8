feed_table_inline_tag(struct table *tbl,
		      char *line, struct table_mode *mode, int width)
{
    check_rowcol(tbl, mode);
    pushdata(tbl, tbl->row, tbl->col, line);
    if (width >= 0) {
	check_minimum0(tbl, width);
	addcontentssize(tbl, width);
	setwidth(tbl, mode);
    }
}