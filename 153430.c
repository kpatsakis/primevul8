callout_tag_entry_raw(ScanEnv* env, CalloutTagTable* t, UChar* name,
                      UChar* name_end, CalloutTagVal entry_val)
{
  int r;
  CalloutTagVal val;

  if (name_end - name <= 0)
    return ONIGERR_INVALID_CALLOUT_TAG_NAME;

  val = callout_tag_find(t, name, name_end);
  if (val >= 0) {
    onig_scan_env_set_error_string(env, ONIGERR_MULTIPLEX_DEFINED_NAME,
                                   name, name_end);
    return ONIGERR_MULTIPLEX_DEFINED_NAME;
  }

  r = onig_st_insert_strend(t, name, name_end, (HashDataType )entry_val);
  if (r < 0) return r;

  return ONIG_NORMAL;
}