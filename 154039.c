setwidth(struct table *t, struct table_mode *mode)
{
    int width = setwidth0(t, mode);
    if (width < 0)
	return;
#ifdef NOWRAP
    if (t->tabattr[t->row][t->col] & HTT_NOWRAP)
	check_minimum0(t, width);
#endif				/* NOWRAP */
    if (mode->pre_mode & (TBLM_NOBR | TBLM_PRE | TBLM_PRE_INT) &&
	mode->nobr_offset >= 0)
	check_minimum0(t, width - mode->nobr_offset);
}