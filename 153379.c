callout_name_table_clear(CalloutNameTable* t)
{
  if (IS_NOT_NULL(t)) {
    onig_st_foreach(t, i_free_callout_name_entry, 0);
  }
  return 0;
}