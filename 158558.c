print_address_error(address_item *addr, FILE *f, uschar *t)
{
int count = Ustrlen(t);
uschar *s = testflag(addr, af_pass_message)? addr->message : NULL;

if (!s && !(s = addr->user_message))
  return;

fprintf(f, "\n    %s", t);

while (*s)
  if (*s == '\\' && s[1] == 'n')
    {
    fprintf(f, "\n    ");
    s += 2;
    count = 0;
    }
  else
    {
    fputc(*s, f);
    count++;
    if (*s++ == ':' && isspace(*s) && count > 45)
      {
      fprintf(f, "\n   ");  /* sic (because space follows) */
      count = 0;
      }
    }
}