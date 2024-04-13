onig_get_args_num_by_callout_args(OnigCalloutArgs* args)
{
  int num;
  CalloutListEntry* e;

  num = args->num;
  e = onig_reg_callout_list_at(args->regex, num);
  if (IS_NULL(e)) return ONIGERR_INVALID_ARGUMENT;
  if (e->of == ONIG_CALLOUT_OF_NAME) {
    return e->u.arg.num;
  }

  return ONIGERR_INVALID_ARGUMENT;
}