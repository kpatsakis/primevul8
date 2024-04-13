callout_tag_find(CalloutTagTable* t, const UChar* name, const UChar* name_end)
{
  CalloutTagVal e;

  e = -1;
  if (IS_NOT_NULL(t)) {
    onig_st_lookup_strend(t, name, name_end, (HashDataType* )((void* )(&e)));
  }
  return e;
}