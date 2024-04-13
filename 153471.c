ext_ensure_tag_table(regex_t* reg)
{
  int r;
  RegexExt* ext;
  CalloutTagTable* t;

  ext = onig_get_regex_ext(reg);
  CHECK_NULL_RETURN_MEMERR(ext);

  if (IS_NULL(ext->tag_table)) {
    r = callout_tag_table_new(&t);
    if (r != ONIG_NORMAL) return r;

    ext->tag_table = t;
  }

  return ONIG_NORMAL;
}