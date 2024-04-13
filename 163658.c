testcase_repoid2str(Pool *pool, Id repoid)
{
  Repo *repo = pool_id2repo(pool, repoid);
  if (repo->name)
    {
      char *r = pool_tmpjoin(pool, repo->name, 0, 0);
      char *rp;
      for (rp = r; *rp; rp++)
	if (*rp == ' ' || *rp == '\t')
	  *rp = '_';
      return r;
    }
  else
    {
      char buf[20];
      sprintf(buf, "#%d", repoid);
      return pool_tmpjoin(pool, buf, 0, 0);
    }
}