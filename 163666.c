str2jobflags(Pool *pool, char *s)	/* modifies the string */
{
  int i, jobflags = 0;
  while (s)
    {
      char *se = strchr(s, ',');
      if (se)
	*se++ = 0;
      for (i = 0; jobflags2str[i].str; i++)
	if (!strcmp(s, jobflags2str[i].str))
	  {
	    jobflags |= jobflags2str[i].flag;
	    break;
	  }
      if (!jobflags2str[i].str)
	pool_error(pool, 0, "str2job: unknown job flag '%s'", s);
      s = se;
    }
  return jobflags;
}