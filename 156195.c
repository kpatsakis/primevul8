onig_get_callout_data(regex_t* reg, OnigMatchParam* mp,
                      int callout_num, int slot,
                      OnigType* type, OnigValue* val)
{
  OnigType t;
  CalloutData* d;

  if (callout_num <= 0) return ONIGERR_INVALID_ARGUMENT;

  d = CALLOUT_DATA_AT_NUM(mp, callout_num);
  if (d->last_match_at_call_counter != mp->match_at_call_counter) {
    xmemset(d, 0, sizeof(*d));
    d->last_match_at_call_counter = mp->match_at_call_counter;
  }

  t = d->slot[slot].type;
  if (IS_NOT_NULL(type)) *type = t;
  if (IS_NOT_NULL(val))  *val  = d->slot[slot].val;
  return (t == ONIG_TYPE_VOID ? 1 : ONIG_NORMAL);
}