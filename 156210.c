onig_initialize_match_param(OnigMatchParam* mp)
{
  mp->match_stack_limit  = MatchStackLimit;
#ifdef USE_RETRY_LIMIT_IN_MATCH
  mp->retry_limit_in_match = RetryLimitInMatch;
#endif

#ifdef USE_CALLOUT
  mp->progress_callout_of_contents   = DefaultProgressCallout;
  mp->retraction_callout_of_contents = DefaultRetractionCallout;
  mp->match_at_call_counter  = 0;
  mp->callout_user_data      = 0;
  mp->callout_data           = 0;
  mp->callout_data_alloc_num = 0;
#endif

  return ONIG_NORMAL;
}