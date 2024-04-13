testcase_getpoolflags(Pool *pool)
{
  const char *str = 0;
  int i, v;
  for (i = 0; poolflags2str[i].str; i++)
    {
      v = pool_get_flag(pool, poolflags2str[i].flag);
      if (v == poolflags2str[i].def)
	continue;
      str = pool_tmpappend(pool, str, v ? " " : " !", poolflags2str[i].str);
    }
  return str ? str + 1 : "";
}