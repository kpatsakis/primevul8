callout_tag_table_new(CalloutTagTable** rt)
{
  CalloutTagTable* t;

  *rt = 0;
  t = onig_st_init_strend_table_with_size(INIT_TAG_NAMES_ALLOC_NUM);
  CHECK_NULL_RETURN_MEMERR(t);

  *rt = t;
  return ONIG_NORMAL;
}