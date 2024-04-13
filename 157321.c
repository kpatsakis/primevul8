void imap_unquote_string (char *s)
{
  char *d = s;

  if (*s == '\"')
    s++;
  else
    return;

  while (*s)
  {
    if (*s == '\"')
    {
      *d = '\0';
      return;
    }
    if (*s == '\\')
    {
      s++;
    }
    if (*s)
    {
      *d = *s;
      d++;
      s++;
    }
  }
  *d = '\0';
}