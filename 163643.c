testcase_resetpoolflags(Pool *pool)
{
  int i;
  for (i = 0; poolflags2str[i].str; i++)
    pool_set_flag(pool, poolflags2str[i].flag, poolflags2str[i].def);
}