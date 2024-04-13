local_part_quote(uschar *lpart)
{
BOOL needs_quote = FALSE;
gstring * g;
uschar *t;

for (t = lpart; !needs_quote && *t != 0; t++)
  {
  needs_quote = !isalnum(*t) && strchr("!#$%&'*+-/=?^_`{|}~", *t) == NULL &&
    (*t != '.' || t == lpart || t[1] == 0);
  }

if (!needs_quote) return lpart;

g = string_catn(NULL, US"\"", 1);

for (;;)
  {
  uschar *nq = US Ustrpbrk(lpart, "\\\"");
  if (nq == NULL)
    {
    g = string_cat(g, lpart);
    break;
    }
  g = string_catn(g, lpart, nq - lpart);
  g = string_catn(g, US"\\", 1);
  g = string_catn(g, nq, 1);
  lpart = nq + 1;
  }

g = string_catn(g, US"\"", 1);
return string_from_gstring(g);
}