static char *rfc2231_get_charset (char *value, char *charset, size_t chslen)
{
  char *t, *u;

  if (!(t = strchr (value, '\'')))
  {
    charset[0] = '\0';
    return value;
  }

  *t = '\0';
  strfcpy (charset, value, chslen);

  if ((u = strchr (t + 1, '\'')))
    return u + 1;
  else
    return t + 1;
}