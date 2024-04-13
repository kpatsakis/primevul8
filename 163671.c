str2resultflags(Pool *pool, char *s)	/* modifies the string! */
{
  int i, resultflags = 0;
  while (s)
    {
      char *se = strchr(s, ',');
      if (se)
	*se++ = 0;
      for (i = 0; resultflags2str[i].str; i++)
	if (!strcmp(s, resultflags2str[i].str))
	  {
	    resultflags |= resultflags2str[i].flag;
	    break;
	  }
      if (!resultflags2str[i].str)
	pool_error(pool, 0, "result: unknown flag '%s'", s);
      s = se;
    }
  return resultflags;
}