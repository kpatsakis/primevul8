onig_get_arg_by_callout_args(OnigCalloutArgs* args, int index,
                             OnigType* type, OnigValue* val)
{
  int num;
  CalloutListEntry* e;

  num = args->num;
  e = onig_reg_callout_list_at(args->regex, num);
  if (IS_NULL(e)) return ONIGERR_INVALID_ARGUMENT;
  if (e->of == ONIG_CALLOUT_OF_NAME) {
    if (IS_NOT_NULL(type)) *type = e->u.arg.types[index];
    if (IS_NOT_NULL(val))  *val  = e->u.arg.vals[index];
    return ONIG_NORMAL;
  }

  return ONIGERR_INVALID_ARGUMENT;
}