testcase_job2str(Pool *pool, Id how, Id what)
{
  char *ret;
  const char *jobstr;
  const char *selstr;
  const char *pkgstr;
  int i, o;
  Id select = how & SOLVER_SELECTMASK;

  for (i = 0; job2str[i].str; i++)
    if ((how & SOLVER_JOBMASK) == job2str[i].job)
      break;
  jobstr = job2str[i].str ? job2str[i].str : "unknown";
  if (select == SOLVER_SOLVABLE)
    {
      selstr = " pkg ";
      pkgstr = testcase_solvid2str(pool, what);
    }
  else if (select == SOLVER_SOLVABLE_NAME)
    {
      selstr = " name ";
      pkgstr = testcase_dep2str(pool, what);
    }
  else if (select == SOLVER_SOLVABLE_PROVIDES)
    {
      selstr = " provides ";
      pkgstr = testcase_dep2str(pool, what);
    }
  else if (select == SOLVER_SOLVABLE_ONE_OF)
    {
      Id p;
      selstr = " oneof ";
      pkgstr = 0;
      while ((p = pool->whatprovidesdata[what++]) != 0)
	{
	  const char *s = testcase_solvid2str(pool, p);
	  if (pkgstr)
	    {
	      pkgstr = pool_tmpappend(pool, pkgstr, " ", s);
	      pool_freetmpspace(pool, s);
	    }
	  else
	    pkgstr = s;
	}
      if (!pkgstr)
	pkgstr = "nothing";
    }
  else if (select == SOLVER_SOLVABLE_REPO)
    {
      Repo *repo = pool_id2repo(pool, what);
      selstr = " repo ";
      if (!repo->name)
	{
          char buf[20];
	  sprintf(buf, "#%d", repo->repoid);
	  pkgstr = pool_tmpjoin(pool, buf, 0, 0);
	}
      else
        pkgstr = pool_tmpjoin(pool, repo->name, 0, 0);
    }
  else if (select == SOLVER_SOLVABLE_ALL)
    {
      selstr = " all ";
      pkgstr = "packages";
    }
  else
    {
      selstr = " unknown ";
      pkgstr = "unknown";
    }
  ret = pool_tmpjoin(pool, jobstr, selstr, pkgstr);
  o = strlen(ret);
  ret = pool_tmpappend(pool, ret, " ", 0);
  for (i = 0; jobflags2str[i].str; i++)
    if ((how & jobflags2str[i].flag) != 0)
      ret = pool_tmpappend(pool, ret, ",", jobflags2str[i].str);
  if (!ret[o + 1])
    ret[o] = 0;
  else
    {
      ret[o + 1] = '[';
      ret = pool_tmpappend(pool, ret, "]", 0);
    }
  return ret;
}