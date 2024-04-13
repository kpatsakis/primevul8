onig_set_retraction_callout_of_match_param(OnigMatchParam* param, OnigCalloutFunc f)
{
#ifdef USE_CALLOUT
  param->retraction_callout_of_contents = f;
  return ONIG_NORMAL;
#else
  return ONIG_NO_SUPPORT_CONFIG;
#endif
}