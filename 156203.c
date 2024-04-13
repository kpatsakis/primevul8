onig_set_callout_data_by_callout_args_self(OnigCalloutArgs* args,
                                           int slot, OnigType type, OnigValue* val)
{
  return onig_set_callout_data(args->regex, args->msa->mp, args->num, slot,
                               type, val);
}