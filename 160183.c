void LibRaw::trimSpaces(char *s)
{
  char *p = s;
  int l = strlen(p);
  if (!l)
    return;
  while (isspace(p[l - 1]))
    p[--l] = 0; /* trim trailing spaces */
  while (*p && isspace(*p))
    ++p, --l;   /* trim leading spaces */
  memmove(s, p, l + 1);
}