callout_tag_table_clear(CalloutTagTable* t)
{
  if (IS_NOT_NULL(t)) {
    onig_st_foreach(t, i_free_callout_tag_entry, 0);
  }
  return 0;
}