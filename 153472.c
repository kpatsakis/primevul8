global_callout_name_table_free(void)
{
  if (IS_NOT_NULL(GlobalCalloutNameTable)) {
    int r = callout_name_table_clear(GlobalCalloutNameTable);
    if (r != 0) return r;

    onig_st_free_table(GlobalCalloutNameTable);
    GlobalCalloutNameTable = 0;
    CalloutNameIDCounter = 0;
  }

  return 0;
}