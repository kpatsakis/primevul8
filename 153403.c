onig_callout_tag_table_free(void* table)
{
  CalloutTagTable* t = (CalloutTagTable* )table;

  if (IS_NOT_NULL(t)) {
    int r = callout_tag_table_clear(t);
    if (r != 0) return r;

    onig_st_free_table(t);
  }

  return 0;
}