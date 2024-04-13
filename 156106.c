string_append(gstring *string, int count, ...)
{
va_list ap;

va_start(ap, count);
while (count-- > 0)
  {
  uschar *t = va_arg(ap, uschar *);
  string = string_cat(string, t);
  }
va_end(ap);

return string;
}