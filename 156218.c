onig_builtin_count(OnigCalloutArgs* args, void* user_data)
{
  (void )onig_check_callout_data_and_clear_old_values(args);

  return onig_builtin_total_count(args, user_data);
}