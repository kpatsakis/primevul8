header_match(uschar *name, BOOL has_addresses, BOOL cond, string_item *strings,
  int count, ...)
{
va_list ap;
string_item *s;
int i;
int slen = Ustrlen(name);

for (s = strings; s != NULL; s = s->next)
  {
  if (one_pattern_match(name, slen, has_addresses, s->text)) return cond;
  }

va_start(ap, count);
for (i = 0; i < count; i++)
  if (one_pattern_match(name, slen, has_addresses, va_arg(ap, uschar *)))
    {
    va_end(ap);
    return cond;
    }
va_end(ap);

return !cond;
}