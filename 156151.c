onig_get_callout_data_by_callout_args_self_dont_clear_old(OnigCalloutArgs* args,
                                                          int slot, OnigType* type,
                                                          OnigValue* val)
{
  return onig_get_callout_data_dont_clear_old(args->regex, args->msa->mp,
                                              args->num, slot, type, val);
}