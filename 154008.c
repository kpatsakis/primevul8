table_close_textarea(struct table *tbl, struct table_mode *mode, int width)
{
    Str tmp = process_n_textarea();
    mode->pre_mode &= ~TBLM_INTXTA;
    mode->end_tag = 0;
    feed_table1(tbl, tmp, mode, width);
}