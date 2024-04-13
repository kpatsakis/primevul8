table_close_anchor0(struct table *tbl, struct table_mode *mode)
{
    if (!(mode->pre_mode & TBLM_ANCHOR))
	return;
    mode->pre_mode &= ~TBLM_ANCHOR;
    if (tbl->tabcontentssize == mode->anchor_offset) {
	check_minimum0(tbl, 1);
	addcontentssize(tbl, 1);
	setwidth(tbl, mode);
    }
    else if (tbl->linfo.prev_spaces > 0 &&
	     tbl->tabcontentssize - 1 == mode->anchor_offset) {
	if (tbl->linfo.prev_spaces > 0)
	    tbl->linfo.prev_spaces = -1;
    }
}