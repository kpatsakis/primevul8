onig_set_match_stack_limit_size_of_match_param(OnigMatchParam* param,
                                               unsigned int limit)
{
  param->match_stack_limit = limit;
  return ONIG_NORMAL;
}