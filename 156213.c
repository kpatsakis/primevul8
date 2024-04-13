onig_set_retry_limit_in_match_of_match_param(OnigMatchParam* param,
                                             unsigned long limit)
{
  param->retry_limit_in_match = limit;
  return ONIG_NORMAL;
}