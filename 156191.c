onig_set_progress_callout(OnigCalloutFunc f)
{
  DefaultProgressCallout = f;
  return ONIG_NORMAL;
}