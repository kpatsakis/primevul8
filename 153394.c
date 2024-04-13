i_free_callout_tag_entry(UChar* key, CalloutTagVal e, void* arg ARG_UNUSED)
{
  xfree(key);
  return ST_DELETE;
}