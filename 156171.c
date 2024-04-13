onig_set_retraction_callout(OnigCalloutFunc f)
{
  DefaultRetractionCallout = f;
  return ONIG_NORMAL;
}