onig_set_callout_data(regex_t* reg, OnigMatchParam* mp,
                      int callout_num, int slot,
                      OnigType type, OnigValue* val)
{
  CalloutData* d;

  if (callout_num <= 0) return ONIGERR_INVALID_ARGUMENT;

  d = CALLOUT_DATA_AT_NUM(mp, callout_num);
  d->slot[slot].type = type;
  d->slot[slot].val  = *val;
  d->last_match_at_call_counter = mp->match_at_call_counter;

  return ONIG_NORMAL;
}