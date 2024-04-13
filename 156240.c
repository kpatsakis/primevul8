onig_free_match_param(OnigMatchParam* p)
{
  if (IS_NOT_NULL(p)) {
    onig_free_match_param_content(p);
    xfree(p);
  }
}