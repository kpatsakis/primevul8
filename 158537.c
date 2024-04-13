decode_control(const uschar *arg, const uschar **pptr, int where, uschar **log_msgptr)
{
int idx, len;
control_def * d;

if (  (idx = find_control(arg, controls_list, nelem(controls_list))) < 0
   || (  arg[len = Ustrlen((d = controls_list+idx)->name)] != 0
      && (!d->has_option || arg[len] != '/')
   )  )
  {
  *log_msgptr = string_sprintf("syntax error in \"control=%s\"", arg);
  return CONTROL_ERROR;
  }

*pptr = arg + len;
return idx;
}