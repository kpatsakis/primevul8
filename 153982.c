clearcontentssize(struct table *t, struct table_mode *mode)
{
    table_close_anchor0(t, mode);
    mode->nobr_offset = 0;
    t->linfo.prev_spaces = -1;
    set_space_to_prevchar(t->linfo.prevchar);
    t->linfo.prev_ctype = PC_ASCII;
    t->linfo.length = 0;
    t->tabcontentssize = 0;
}