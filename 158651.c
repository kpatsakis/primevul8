debug_print_string(uschar *debug_string)
{
if (!debug_string) return;
HDEBUG(D_any|D_v)
  {
  uschar *s = expand_string(debug_string);
  if (!s)
    debug_printf("failed to expand debug_output \"%s\": %s\n", debug_string,
      expand_string_message);
  else if (s[0] != 0)
    debug_printf("%s%s", s, (s[Ustrlen(s)-1] == '\n')? "" : "\n");
  }
}