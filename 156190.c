onig_set_callout_user_data_of_match_param(OnigMatchParam* param, void* user_data)
{
#ifdef USE_CALLOUT
  param->callout_user_data = user_data;
  return ONIG_NORMAL;
#else
  return ONIG_NO_SUPPORT_CONFIG;
#endif
}