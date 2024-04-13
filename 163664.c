testcase_str2job(Pool *pool, const char *str, Id *whatp)
{
  int i;
  Id job, jobsel;
  Id what;
  char *s;
  char **pieces = 0;
  int npieces = 0;

  *whatp = 0;
  /* so we can patch it */
  s = pool_tmpjoin(pool, str, 0, 0);
  /* split it in pieces */
  for (;;)
    {
      while (*s == ' ' || *s == '\t')
	s++;
      if (!*s)
	break;
      pieces = solv_extend(pieces, npieces, 1, sizeof(*pieces), 7);
      pieces[npieces++] = s;
      while (*s && *s != ' ' && *s != '\t')
	s++;
      if (*s)
	*s++ = 0;
    }
  if (npieces < 3)
    {
      pool_error(pool, -1, "str2job: bad line '%s'", str);
      solv_free(pieces);
      return -1;
    }

  for (i = 0; job2str[i].str; i++)
    if (!strcmp(pieces[0], job2str[i].str))
      break;
  if (!job2str[i].str)
    {
      pool_error(pool, -1, "str2job: unknown job '%s'", str);
      solv_free(pieces);
      return -1;
    }
  job = job2str[i].job;
  what = 0;
  if (npieces > 3)
    {
      char *flags = pieces[npieces - 1];
      if (*flags == '[' && flags[strlen(flags) - 1] == ']')
	{
	  npieces--;
	  flags++;
	  flags[strlen(flags) - 1] = 0;
	  job |= str2jobflags(pool, flags);
	}
    }
  jobsel = testcase_str2jobsel(pool, "str2job", pieces + 1, npieces - 1, &what);
  solv_free(pieces);
  if (jobsel == -1)
    return -1;
  *whatp = what;
  return job | jobsel;
}