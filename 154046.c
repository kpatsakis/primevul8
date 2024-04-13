table_close_select(struct table *tbl, struct table_mode *mode, int width)
{
    Str tmp = process_n_select();
    mode->pre_mode &= ~TBLM_INSELECT;
    mode->end_tag = 0;
    feed_table1(tbl, tmp, mode, width);
}