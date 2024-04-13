onig_check_callout_data_and_clear_old_values(OnigCalloutArgs* args)
{
  OnigMatchParam* mp;
  int num;
  CalloutData* d;

  mp  = args->msa->mp;
  num = args->num;

  d = CALLOUT_DATA_AT_NUM(mp, num);
  if (d->last_match_at_call_counter != mp->match_at_call_counter) {
    xmemset(d, 0, sizeof(*d));
    d->last_match_at_call_counter = mp->match_at_call_counter;
    return d->last_match_at_call_counter;
  }

  return 0;
}