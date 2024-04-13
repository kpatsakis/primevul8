name_to_group_numbers(ScanEnv* env, const UChar* name, const UChar* name_end,
                      int** nums)
{
  regex_t* reg;
  NameEntry* e;

  reg = env->reg;
  e = name_find(reg, name, name_end);

  if (IS_NULL(e)) {
    onig_scan_env_set_error_string(env, ONIGERR_UNDEFINED_NAME_REFERENCE,
                                   (UChar* )name, (UChar* )name_end);
    return ONIGERR_UNDEFINED_NAME_REFERENCE;
  }

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