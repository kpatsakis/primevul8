onig_set_callout_data_by_tag(regex_t* reg, OnigMatchParam* mp,
                             const UChar* tag, const UChar* tag_end, int slot,
                             OnigType type, OnigValue* val)
{
  int num;

  num = onig_get_callout_num_by_tag(reg, tag, tag_end);
  if (num < 0)  return num;
  if (num == 0) return ONIGERR_INVALID_CALLOUT_TAG_NAME;

  return onig_set_callout_data(reg, mp, num, slot, type, val);
}