begin_cell(struct table *t, struct table_mode *mode)
{
    clearcontentssize(t, mode);
    mode->indent_level = 0;
    mode->nobr_level = 0;
    mode->pre_mode = 0;
    t->indent = 0;
    t->flag |= TBL_IN_COL;

    if (t->suspended_data) {
	check_row(t, t->row);
	if (t->tabdata[t->row][t->col] == NULL)
	    t->tabdata[t->row][t->col] = newGeneralList();
	appendGeneralList(t->tabdata[t->row][t->col],
			  (GeneralList *)t->suspended_data);
	t->suspended_data = NULL;
    }
}