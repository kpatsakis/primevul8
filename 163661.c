testcase_write_mangled(Solver *solv, const char *dir, int resultflags, const char *testcasename, const char *resultname)
{
  Pool *pool = solv->pool;
  Repo *repo;
  int i;
  Id arch, repoid;
  Id lowscore;
  FILE *fp;
  Strqueue sq;
  char *cmd, *out, *result;
  const char *s;

  if (!testcasename)
    testcasename = "testcase.t";
  if (!resultname)
    resultname = "solver.result";

#ifdef _WIN32
  if (mkdir(dir) && errno != EEXIST)
#else
  if (mkdir(dir, 0777) && errno != EEXIST)
#endif
    return pool_error(solv->pool, 0, "testcase_write: could not create directory '%s'", dir);
  strqueue_init(&sq);
  FOR_REPOS(repoid, repo)
    {
      const char *name = testcase_repoid2str(pool, repoid);
      char priobuf[50];
      if (repo->subpriority)
	sprintf(priobuf, "%d.%d", repo->priority, repo->subpriority);
      else
	sprintf(priobuf, "%d", repo->priority);
#if !defined(WITHOUT_COOKIEOPEN) && defined(ENABLE_ZLIB_COMPRESSION)
      out = pool_tmpjoin(pool, name, ".repo", ".gz");
#else
      out = pool_tmpjoin(pool, name, ".repo", 0);
#endif
      for (i = 0; out[i]; i++)
	if (out[i] == '/')
	  out[i] = '_';
      cmd = pool_tmpjoin(pool, "repo ", name, " ");
      cmd = pool_tmpappend(pool, cmd, priobuf, " ");
      cmd = pool_tmpappend(pool, cmd, "testtags ", out);
      strqueue_push(&sq, cmd);
      out = pool_tmpjoin(pool, dir, "/", out);
      if (!(fp = solv_xfopen(out, "w")))
	{
	  pool_error(solv->pool, 0, "testcase_write: could not open '%s' for writing", out);
	  strqueue_free(&sq);
	  return 0;
	}
      testcase_write_testtags(repo, fp);
      if (fclose(fp))
	{
	  pool_error(solv->pool, 0, "testcase_write: write error");
	  strqueue_free(&sq);
	  return 0;
	}
    }
  /* hmm, this is not optimal... we currently search for the lowest score */
  lowscore = 0;
  arch = pool->solvables[SYSTEMSOLVABLE].arch;
  for (i = 0; i < pool->lastarch; i++)
    {
      if (pool->id2arch[i] == 1 && !lowscore)
	arch = i;
      if (pool->id2arch[i] > 0x10000 && (!lowscore || pool->id2arch[i] < lowscore))
	{
	  arch = i;
	  lowscore = pool->id2arch[i];
	}
    }
  cmd = pool_tmpjoin(pool, "system ", pool->lastarch ? pool_id2str(pool, arch) : "-", 0);
  for (i = 0; disttype2str[i].str != 0; i++)
    if (pool->disttype == disttype2str[i].type)
      break;
  pool_tmpappend(pool, cmd, " ", disttype2str[i].str ? disttype2str[i].str : "unknown");
  if (pool->installed)
    cmd = pool_tmpappend(pool, cmd, " ", testcase_repoid2str(pool, pool->installed->repoid));
  strqueue_push(&sq, cmd);
  s = testcase_getpoolflags(solv->pool);
  if (*s)
    {
      cmd = pool_tmpjoin(pool, "poolflags ", s, 0);
      strqueue_push(&sq, cmd);
    }

  if (pool->vendorclasses)
    {
      cmd = 0;
      for (i = 0; pool->vendorclasses[i]; i++)
	{
	  cmd = pool_tmpappend(pool, cmd ? cmd : "vendorclass", " ", pool->vendorclasses[i]);
	  if (!pool->vendorclasses[i + 1])
	    {
	      strqueue_push(&sq, cmd);
	      cmd = 0;
	      i++;
	    }
	}
    }

  /* dump disabled packages (must come before the namespace/job lines) */
  if (pool->considered)
    {
      Id p;
      FOR_POOL_SOLVABLES(p)
	if (!MAPTST(pool->considered, p))
	  {
	    cmd = pool_tmpjoin(pool, "disable pkg ", testcase_solvid2str(pool, p), 0);
	    strqueue_push(&sq, cmd);
	  }
    }

  s = testcase_getsolverflags(solv);
  if (*s)
    {
      cmd = pool_tmpjoin(pool, "solverflags ", s, 0);
      strqueue_push(&sq, cmd);
    }

  /* now dump all the ns callback values we know */
  if (pool->nscallback)
    {
      Id rid;
      int d;
      for (rid = 1; rid < pool->nrels; rid++)
	{
	  Reldep *rd = pool->rels + rid;
	  if (rd->flags != REL_NAMESPACE || rd->name == NAMESPACE_OTHERPROVIDERS || rd->name == NAMESPACE_SPLITPROVIDES)
	    continue;
	  /* evaluate all namespace ids, skip empty results */
	  d = pool_whatprovides(pool, MAKERELDEP(rid));
	  if (!d || !pool->whatprovidesdata[d])
	    continue;
	  cmd = pool_tmpjoin(pool, "namespace ", pool_id2str(pool, rd->name), "(");
	  cmd = pool_tmpappend(pool, cmd, pool_id2str(pool, rd->evr), ")");
	  for (;  pool->whatprovidesdata[d]; d++)
	    cmd = pool_tmpappend(pool, cmd, " ", testcase_solvid2str(pool, pool->whatprovidesdata[d]));
	  strqueue_push(&sq, cmd);
	}
    }

  for (i = 0; i < solv->job.count; i += 2)
    {
      cmd = (char *)testcase_job2str(pool, solv->job.elements[i], solv->job.elements[i + 1]);
      cmd = pool_tmpjoin(pool, "job ", cmd, 0);
      strqueue_push(&sq, cmd);
    }

  if ((resultflags & ~TESTCASE_RESULT_REUSE_SOLVER) != 0)
    {
      cmd = 0;
      for (i = 0; resultflags2str[i].str; i++)
	if ((resultflags & resultflags2str[i].flag) != 0)
	  cmd = pool_tmpappend(pool, cmd, cmd ? "," : 0, resultflags2str[i].str);
      cmd = pool_tmpjoin(pool, "result ", cmd ? cmd : "?", 0);
      cmd = pool_tmpappend(pool, cmd, " ", resultname);
      strqueue_push(&sq, cmd);
      result = testcase_solverresult(solv, resultflags);
      if (!strcmp(resultname, "<inline>"))
	{
	  Strqueue rsq;
	  strqueue_init(&rsq);
	  strqueue_split(&rsq, result);
	  for (i = 0; i < rsq.nstr; i++)
	    {
	      cmd = pool_tmpjoin(pool, "#>", rsq.str[i], 0);
	      strqueue_push(&sq, cmd);
	    }
	  strqueue_free(&rsq);
	}
      else
	{
	  out = pool_tmpjoin(pool, dir, "/", resultname);
	  if (!(fp = fopen(out, "w")))
	    {
	      pool_error(solv->pool, 0, "testcase_write: could not open '%s' for writing", out);
	      solv_free(result);
	      strqueue_free(&sq);
	      return 0;
	    }
	  if (result && *result && fwrite(result, strlen(result), 1, fp) != 1)
	    {
	      pool_error(solv->pool, 0, "testcase_write: write error");
	      solv_free(result);
	      strqueue_free(&sq);
	      fclose(fp);
	      return 0;
	    }
	  if (fclose(fp))
	    {
	      pool_error(solv->pool, 0, "testcase_write: write error");
	      solv_free(result);
	      strqueue_free(&sq);
	      return 0;
	    }
	}
      solv_free(result);
    }

  result = strqueue_join(&sq);
  strqueue_free(&sq);
  out = pool_tmpjoin(pool, dir, "/", testcasename);
  if (!(fp = fopen(out, "w")))
    {
      pool_error(solv->pool, 0, "testcase_write: could not open '%s' for writing", out);
      solv_free(result);
      return 0;
    }
  if (*result && fwrite(result, strlen(result), 1, fp) != 1)
    {
      pool_error(solv->pool, 0, "testcase_write: write error");
      solv_free(result);
      fclose(fp);
      return 0;
    }
  if (fclose(fp))
    {
      pool_error(solv->pool, 0, "testcase_write: write error");
      solv_free(result);
      return 0;
    }
  solv_free(result);
  return 1;
}