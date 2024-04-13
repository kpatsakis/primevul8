onig_global_callout_names_free(void)
{
  free_callout_func_list(GlobalCalloutNameList);
  GlobalCalloutNameList = 0;

  global_callout_name_table_free();
  return ONIG_NORMAL;
}