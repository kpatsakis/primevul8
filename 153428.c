onig_name_to_backref_number(regex_t* reg, const UChar* name,
                            const UChar* name_end, OnigRegion *region)
{
  int i, n, *nums;

  n = onig_name_to_group_numbers(reg, name, name_end, &nums);
  if (n < 0)
    return n;
  else if (n == 0)
    return ONIGERR_PARSER_BUG;
  else if (n == 1)
    return nums[0];
  else {
    if (IS_NOT_NULL(region)) {
      for (i = n - 1; i >= 0; i--) {
        if (region->beg[nums[i]] != ONIG_REGION_NOTPOS)
          return nums[i];
      }
    }
    return nums[n - 1];
  }
}