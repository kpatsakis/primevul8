void trimSpaces(char *s)
{
  char *p = s;
  if (!strncasecmp(p, "NO=", 3))
    p = p + 3; /* fix for Nikon D70, D70s */
  int l = strlen(p);
  if (!l)
    return;
  while (isspace(p[l - 1]))
    p[--l] = 0; /* trim trailing spaces */
  while (*p && isspace(*p))
    ++p, --l; /* trim leading spaces */
  memmove(s, p, l + 1);
}