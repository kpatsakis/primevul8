onig_free_match_param_content(OnigMatchParam* p)
{
#ifdef USE_CALLOUT
  if (IS_NOT_NULL(p->callout_data)) {
    xfree(p->callout_data);
    p->callout_data = 0;
  }
#endif
}