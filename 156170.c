onig_builtin_max(OnigCalloutArgs* args, void* user_data ARG_UNUSED)
{
  int r;
  int slot;
  long max_val;
  OnigCodePoint count_type;
  OnigType  type;
  OnigValue val;
  OnigValue aval;

  (void )onig_check_callout_data_and_clear_old_values(args);

  slot = 0;
  r = onig_get_callout_data_by_callout_args_self(args, slot, &type, &val);
  if (r < ONIG_NORMAL)
    return r;
  else if (r > ONIG_NORMAL) {
    /* type == void: initial state */
    type  = ONIG_TYPE_LONG;
    val.l = 0;
  }

  r = onig_get_arg_by_callout_args(args, 0, &type, &aval);
  if (r != ONIG_NORMAL) return r;
  if (type == ONIG_TYPE_TAG) {
    r = onig_get_callout_data_by_callout_args(args, aval.tag, 0, &type, &aval);
    if (r < ONIG_NORMAL) return r;
    else if (r > ONIG_NORMAL)
      max_val = 0L;
    else
      max_val = aval.l;
  }
  else { /* LONG */
    max_val = aval.l;
  }

  r = onig_get_arg_by_callout_args(args, 1, &type, &aval);
  if (r != ONIG_NORMAL) return r;

  count_type = aval.c;
  if (count_type != '>' && count_type != 'X' && count_type != '<')
    return ONIGERR_INVALID_CALLOUT_ARG;

  if (args->in == ONIG_CALLOUT_IN_RETRACTION) {
    if (count_type == '<') {
      if (val.l >= max_val) return ONIG_CALLOUT_FAIL;
      val.l++;
    }
    else if (count_type == 'X')
      val.l--;
  }
  else {
    if (count_type != '<') {
      if (val.l >= max_val) return ONIG_CALLOUT_FAIL;
      val.l++;
    }
  }

  r = onig_set_callout_data_by_callout_args_self(args, slot, ONIG_TYPE_LONG, &val);
  if (r != ONIG_NORMAL) return r;

  return ONIG_CALLOUT_SUCCESS;
}