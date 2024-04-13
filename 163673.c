testcase_getsolverflags(Solver *solv)
{
  Pool *pool = solv->pool;
  const char *str = 0;
  int i, v;
  for (i = 0; solverflags2str[i].str; i++)
    {
      v = solver_get_flag(solv, solverflags2str[i].flag);
      if (v == solverflags2str[i].def)
	continue;
      str = pool_tmpappend(pool, str, v ? " " : " !", solverflags2str[i].str);
    }
  return str ? str + 1 : "";
}