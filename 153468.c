onig_get_callout_in_by_name_id(int name_id)
{
  if (name_id < 0 || name_id >= GlobalCalloutNameList->n)
    return 0;

  return GlobalCalloutNameList->v[name_id].in;
}