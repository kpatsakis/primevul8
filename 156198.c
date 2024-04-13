onig_get_callout_data_dont_clear_old(regex_t* reg, OnigMatchParam* mp,
                                     int callout_num, int slot,
                                     OnigType* type, OnigValue* val)
{
  OnigType t;
  CalloutData* d;

  if (callout_num <= 0) return ONIGERR_INVALID_ARGUMENT;

  d = CALLOUT_DATA_AT_NUM(mp, callout_num);
  t = d->slot[slot].type;
  if (IS_NOT_NULL(type)) *type = t;
  if (IS_NOT_NULL(val))  *val  = d->slot[slot].val;
  return (t == ONIG_TYPE_VOID ? 1 : ONIG_NORMAL);
}