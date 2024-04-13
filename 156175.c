onig_get_used_stack_size_in_callout(OnigCalloutArgs* a, int* used_num, int* used_bytes)
{
  int n;

  n = (int )(a->stk - a->stk_base);

  if (used_num != 0)
    *used_num = n;

  if (used_bytes != 0)
    *used_bytes = n * sizeof(StackType);

  return ONIG_NORMAL;
}