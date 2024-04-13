onig_set_callout_data_by_callout_args(OnigCalloutArgs* args,
                                      int callout_num, int slot,
                                      OnigType type, OnigValue* val)
{
  return onig_set_callout_data(args->regex, args->msa->mp, callout_num, slot,
                               type, val);
}