testcase_setpoolflags(Pool *pool, const char *str)
{
  const char *p = str, *s;
  int i, v;
  for (;;)
    {
      while (*p == ' ' || *p == '\t' || *p == ',')
	p++;
      v = 1;
      if (*p == '!')
	{
	  p++;
	  v = 0;
	}
      if (!*p)
	break;
      s = p;
      while (*p && *p != ' ' && *p != '\t' && *p != ',')
	p++;
      for (i = 0; poolflags2str[i].str; i++)
	if (!strncmp(poolflags2str[i].str, s, p - s) && poolflags2str[i].str[p - s] == 0)
	  break;
      if (!poolflags2str[i].str)
        return pool_error(pool, 0, "setpoolflags: unknown flag '%.*s'", (int)(p - s), s);
      pool_set_flag(pool, poolflags2str[i].flag, v);
    }
  return 1;
}