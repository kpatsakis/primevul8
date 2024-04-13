testcase_setsolverflags(Solver *solv, const char *str)
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
      for (i = 0; solverflags2str[i].str; i++)
	if (!strncmp(solverflags2str[i].str, s, p - s) && solverflags2str[i].str[p - s] == 0)
	  break;
      if (!solverflags2str[i].str)
	return pool_error(solv->pool, 0, "setsolverflags: unknown flag '%.*s'", (int)(p - s), s);
      if (solver_set_flag(solv, solverflags2str[i].flag, v) == -1)
        return pool_error(solv->pool, 0, "setsolverflags: unsupported flag '%s'", solverflags2str[i].str);
    }
  return 1;
}