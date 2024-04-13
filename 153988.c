feed_table1(struct table *tbl, Str tok, struct table_mode *mode, int width)
{
    Str tokbuf;
    int status;
    char *line;
    if (!tok)
	return;
    tokbuf = Strnew();
    status = R_ST_NORMAL;
    line = tok->ptr;
    while (read_token
	   (tokbuf, &line, &status, mode->pre_mode & TBLM_PREMODE, 0))
	feed_table(tbl, tokbuf->ptr, mode, width, TRUE);
}