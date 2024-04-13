_g_locale_charset_unalias (const char *codeset)
{
  const char *aliases;

  if (codeset == NULL)
    /* The canonical name cannot be determined.  */
    codeset = "";

  /* Resolve alias. */
  for (aliases = _g_locale_get_charset_aliases ();
       *aliases != '\0';
       aliases += strlen (aliases) + 1, aliases += strlen (aliases) + 1)
    if (strcmp (codeset, aliases) == 0
	|| (aliases[0] == '*' && aliases[1] == '\0'))
      {
	codeset = aliases + strlen (aliases) + 1;
	break;
      }

  /* Don't return an empty string.  GNU libc and GNU libiconv interpret
     the empty string as denoting "the locale's character encoding",
     thus GNU libiconv would call this function a second time.  */
  if (codeset[0] == '\0')
    codeset = "ASCII";

  return codeset;
}