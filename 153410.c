onig_name_to_group_numbers(regex_t* reg, const UChar* name,
                           const UChar* name_end, int** nums)
{
  NameEntry* e = name_find(reg, name, name_end);

  if (IS_NULL(e)) return ONIGERR_UNDEFINED_NAME_REFERENCE;

  switch (e->back_num) {
  case 0:
    break;
  case 1:
    *nums = &(e->back_ref1);
    break;
  default:
    *nums = e->back_refs;
    break;
  }
  return e->back_num;
}