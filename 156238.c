onig_builtin_total_count(OnigCalloutArgs* args, void* user_data ARG_UNUSED)
{
  int r;
  int slot;
  OnigType  type;
  OnigValue val;
  OnigValue aval;
  OnigCodePoint count_type;

  r = onig_get_arg_by_callout_args(args, 0, &type, &aval);
  if (r != ONIG_NORMAL) return r;

  count_type = aval.c;
  if (count_type != '>' && count_type != 'X' && count_type != '<')
    return ONIGERR_INVALID_CALLOUT_ARG;

  r = onig_get_callout_data_by_callout_args_self_dont_clear_old(args, 0,
                                                                &type, &val);
  if (r < ONIG_NORMAL)
    return r;
  else if (r > ONIG_NORMAL) {
    /* type == void: initial state */
    val.l = 0;
  }

  if (args->in == ONIG_CALLOUT_IN_RETRACTION) {
    slot = 2;
    if (count_type == '<')
      val.l++;
    else if (count_type == 'X')
      val.l--;
  }
  else {
    slot = 1;
    if (count_type != '<')
      val.l++;
  }

  r = onig_set_callout_data_by_callout_args_self(args, 0, ONIG_TYPE_LONG, &val);
  if (r != ONIG_NORMAL) return r;

  /* slot 1: in progress counter, slot 2: in retraction counter */
  r = onig_get_callout_data_by_callout_args_self_dont_clear_old(args, slot,
                                                                &type, &val);
  if (r < ONIG_NORMAL)
    return r;
  else if (r > ONIG_NORMAL) {
    val.l = 0;
  }

  val.l++;
  r = onig_set_callout_data_by_callout_args_self(args, slot, ONIG_TYPE_LONG, &val);
  if (r != ONIG_NORMAL) return r;

  return ONIG_CALLOUT_SUCCESS;
}