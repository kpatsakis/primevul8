onig_get_contents_end_by_callout_args(OnigCalloutArgs* args)
{
  int num;
  CalloutListEntry* e;

  num = args->num;
  e = onig_reg_callout_list_at(args->regex, num);
  if (IS_NULL(e)) return 0;
  if (e->of == ONIG_CALLOUT_OF_CONTENTS) {
    return e->u.content.end;
  }

  return 0;
}