testcase_write(Solver *solv, const char *dir, int resultflags, const char *testcasename, const char *resultname)
{
  Pool *pool = solv->pool;
  int r, repoid;
  const char **names;

  /* mangle repo names so that there are no conflicts */
  names = testcase_mangle_repo_names(pool);
  swap_repo_names(pool, names);
  r = testcase_write_mangled(solv, dir, resultflags, testcasename, resultname);
  swap_repo_names(pool, names);
  for (repoid = 1; repoid < pool->nrepos; repoid++)
    solv_free((void *)names[repoid]);
  solv_free((void *)names);
  return r;
}