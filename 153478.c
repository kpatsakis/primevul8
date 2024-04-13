get_callout_name_id_by_name(OnigEncoding enc, int is_not_single,
                            UChar* name, UChar* name_end, int* rid)
{
  int r;
  CalloutNameEntry* e;

  if (! is_allowed_callout_name(enc, name, name_end)) {
    return ONIGERR_INVALID_CALLOUT_NAME;
  }

  e = callout_name_find(enc, is_not_single, name, name_end);
  if (IS_NULL(e)) {
    return ONIGERR_UNDEFINED_CALLOUT_NAME;
  }

  r = ONIG_NORMAL;
  *rid = e->id;

  return r;
}