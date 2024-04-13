_g_locale_charset_raw (void)
{
  const char *codeset;

#if !(defined WIN32_NATIVE || defined OS2)

# if HAVE_LANGINFO_CODESET

  /* Most systems support nl_langinfo (CODESET) nowadays.  */
  codeset = nl_langinfo (CODESET);

#  ifdef __CYGWIN__
  /* Cygwin 2006 does not have locales.  nl_langinfo (CODESET) always
     returns "US-ASCII".  As long as this is not fixed, return the suffix
     of the locale name from the environment variables (if present) or
     the codepage as a number.  */
  if (codeset != NULL && strcmp (codeset, "US-ASCII") == 0)
    {
      const char *locale;
      static char buf[2 + 10 + 1];

      locale = getenv ("LC_ALL");
      if (locale == NULL || locale[0] == '\0')
	{
	  locale = getenv ("LC_CTYPE");
	  if (locale == NULL || locale[0] == '\0')
	    locale = getenv ("LANG");
	}
      if (locale != NULL && locale[0] != '\0')
	{
	  /* If the locale name contains an encoding after the dot, return
	     it.  */
	  const char *dot = strchr (locale, '.');

	  if (dot != NULL)
	    {
	      const char *modifier;

	      dot++;
	      /* Look for the possible @... trailer and remove it, if any.  */
	      modifier = strchr (dot, '@');
	      if (modifier == NULL)
		return dot;
	      if (modifier - dot < sizeof (buf))
		{
		  memcpy (buf, dot, modifier - dot);
		  buf [modifier - dot] = '\0';
		  return buf;
		}
	    }
	}

      /* Woe32 has a function returning the locale's codepage as a number.  */
      sprintf (buf, "CP%u", GetACP ());
      codeset = buf;
    }
#  endif

# else

  /* On old systems which lack it, use setlocale or getenv.  */
  const char *locale = NULL;

  /* But most old systems don't have a complete set of locales.  Some
     (like SunOS 4 or DJGPP) have only the C locale.  Therefore we don't
     use setlocale here; it would return "C" when it doesn't support the
     locale name the user has set.  */
#  if 0
  locale = setlocale (LC_CTYPE, NULL);
#  endif
  if (locale == NULL || locale[0] == '\0')
    {
      locale = getenv ("LC_ALL");
      if (locale == NULL || locale[0] == '\0')
	{
	  locale = getenv ("LC_CTYPE");
	  if (locale == NULL || locale[0] == '\0')
	    locale = getenv ("LANG");
	}
    }

  /* On some old systems, one used to set locale = "iso8859_1". On others,
     you set it to "language_COUNTRY.charset". In any case, we resolve it
     through the charset.alias file.  */
  codeset = locale;

# endif

#elif defined WIN32_NATIVE

  static char buf[2 + 10 + 1];

  /* Woe32 has a function returning the locale's codepage as a number.  */
  sprintf (buf, "CP%u", GetACP ());
  codeset = buf;

#elif defined OS2

  const char *locale;
  static char buf[2 + 10 + 1];
  ULONG cp[3];
  ULONG cplen;

  /* Allow user to override the codeset, as set in the operating system,
     with standard language environment variables.  */
  locale = getenv ("LC_ALL");
  if (locale == NULL || locale[0] == '\0')
    {
      locale = getenv ("LC_CTYPE");
      if (locale == NULL || locale[0] == '\0')
	locale = getenv ("LANG");
    }
  if (locale != NULL && locale[0] != '\0')
    {
      /* If the locale name contains an encoding after the dot, return it.  */
      const char *dot = strchr (locale, '.');

      if (dot != NULL)
	{
	  const char *modifier;

	  dot++;
	  /* Look for the possible @... trailer and remove it, if any.  */
	  modifier = strchr (dot, '@');
	  if (modifier == NULL)
	    return dot;
	  if (modifier - dot < sizeof (buf))
	    {
	      memcpy (buf, dot, modifier - dot);
	      buf [modifier - dot] = '\0';
	      return buf;
	    }
	}

      /* Resolve through the charset.alias file.  */
      codeset = locale;
    }
  else
    {
      /* OS/2 has a function returning the locale's codepage as a number.  */
      if (DosQueryCp (sizeof (cp), cp, &cplen))
	codeset = "";
      else
	{
	  sprintf (buf, "CP%u", cp[0]);
	  codeset = buf;
	}
    }

#endif

  return codeset;
}