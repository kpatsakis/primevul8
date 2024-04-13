onig_get_capture_range_in_callout(OnigCalloutArgs* a, int mem_num, int* begin, int* end)
{
  OnigRegex    reg;
  const UChar* str;
  StackType*   stk_base;
  int i;
  StackIndex* mem_start_stk;
  StackIndex* mem_end_stk;

  i = mem_num;
  reg = a->regex;
  str = a->string;
  stk_base = a->stk_base;
  mem_start_stk = a->mem_start_stk;
  mem_end_stk   = a->mem_end_stk;

  if (i > 0) {
    if (a->mem_end_stk[i] != INVALID_STACK_INDEX) {
      *begin = (int )(STACK_MEM_START(reg, i) - str);
      *end   = (int )(STACK_MEM_END(reg, i)   - str);
    }
    else {
      *begin = *end = ONIG_REGION_NOTPOS;
    }
  }
  else
    return ONIGERR_INVALID_ARGUMENT;

  return ONIG_NORMAL;
}