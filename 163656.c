str2selflags(Pool *pool, char *s)	/* modifies the string! */
{
  int i, selflags = 0;
  while (s)
    {
      char *se = strchr(s, ',');
      if (se)
	*se++ = 0;
      for (i = 0; selflags2str[i].str; i++)
	if (!strcmp(s, selflags2str[i].str))
	  {
	    selflags |= selflags2str[i].flag;
	    break;
	  }
      if (!selflags2str[i].str)
	pool_error(pool, 0, "str2job: unknown selection flag '%s'", s);
      s = se;
    }
  return selflags;
}