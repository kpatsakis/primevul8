onig_builtin_monitor(OnigCalloutArgs* args, void* user_data)
{
  int r;
  int num;
  size_t tag_len;
  const UChar* start;
  const UChar* right;
  const UChar* current;
  const UChar* string;
  const UChar* strend;
  const UChar* tag_start;
  const UChar* tag_end;
  regex_t* reg;
  OnigCalloutIn in;
  OnigType type;
  OnigValue val;
  char buf[20];
  FILE* fp;

  fp = OutFp;

  r = onig_get_arg_by_callout_args(args, 0, &type, &val);
  if (r != ONIG_NORMAL) return r;

  in = onig_get_callout_in_by_callout_args(args);
  if (in == ONIG_CALLOUT_IN_PROGRESS) {
    if (val.c == '<')
      return ONIG_CALLOUT_SUCCESS;
  }
  else {
    if (val.c != 'X' && val.c != '<')
      return ONIG_CALLOUT_SUCCESS;
  }

  num       = onig_get_callout_num_by_callout_args(args);
  start     = onig_get_start_by_callout_args(args);
  right     = onig_get_right_range_by_callout_args(args);
  current   = onig_get_current_by_callout_args(args);
  string    = onig_get_string_by_callout_args(args);
  strend    = onig_get_string_end_by_callout_args(args);
  reg       = onig_get_regex_by_callout_args(args);
  tag_start = onig_get_callout_tag_start(reg, num);
  tag_end   = onig_get_callout_tag_end(reg, num);

  if (tag_start == 0)
    xsnprintf(buf, sizeof(buf), "#%d", num);
  else {
    /* CAUTION: tag string is not terminated with NULL. */
    int i;

    tag_len = tag_end - tag_start;
    if (tag_len >= sizeof(buf)) tag_len = sizeof(buf) - 1;
    for (i = 0; i < tag_len; i++) buf[i] = tag_start[i];
    buf[tag_len] = '\0';
  }

  fprintf(fp, "ONIG-MONITOR: %-4s %s at: %d [%d - %d] len: %d\n",
          buf,
          in == ONIG_CALLOUT_IN_PROGRESS ? "=>" : "<=",
          (int )(current - string),
          (int )(start   - string),
          (int )(right   - string),
          (int )(strend  - string));
  fflush(fp);

  return ONIG_CALLOUT_SUCCESS;
}