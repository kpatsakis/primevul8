onig_new_match_param(void)
{
  OnigMatchParam* p;

  p = (OnigMatchParam* )xmalloc(sizeof(*p));
  if (IS_NOT_NULL(p)) {
    onig_initialize_match_param(p);
  }

  return p;
}