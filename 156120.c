string_dequote(const uschar **sptr)
{
const uschar *s = *sptr;
uschar *t, *yield;

/* First find the end of the string */

if (*s != '\"')
  while (*s != 0 && !isspace(*s)) s++;
else
  {
  s++;
  while (*s && *s != '\"')
    {
    if (*s == '\\') (void)string_interpret_escape(&s);
    s++;
    }
  if (*s) s++;
  }

/* Get enough store to copy into */

t = yield = store_get(s - *sptr + 1);
s = *sptr;

/* Do the copy */

if (*s != '\"')
  {
  while (*s != 0 && !isspace(*s)) *t++ = *s++;
  }
else
  {
  s++;
  while (*s != 0 && *s != '\"')
    {
    if (*s == '\\') *t++ = string_interpret_escape(&s);
      else *t++ = *s;
    s++;
    }
  if (*s != 0) s++;
  }

/* Update the pointer and return the terminated copy */

*sptr = s;
*t = 0;
return yield;
}