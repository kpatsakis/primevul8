testcase_str2jobsel(Pool *pool, const char *caller, char **pieces, int npieces, Id *whatp)
{
  Id job, what;
  if (!strcmp(pieces[0], "pkg") && npieces == 2)
    {
      job = SOLVER_SOLVABLE;
      what = testcase_str2solvid(pool, pieces[1]);
      if (!what)
	return pool_error(pool, -1, "%s: unknown package '%s'", caller, pieces[1]);
    }
  else if (!strcmp(pieces[0], "name") || !strcmp(pieces[0], "provides"))
    {
      /* join em again for dep2str... */
      char *sp;
      for (sp = pieces[1]; sp < pieces[npieces - 1]; sp++)
	if (*sp == 0)
	  *sp = ' ';
      what = 0;
      if (pieces[0][0] == 'p' && strncmp(pieces[1], "namespace:", 10) == 0)
	{
	  char *spe = strchr(pieces[1], '(');
	  int l = strlen(pieces[1]);
	  if (spe && pieces[1][l - 1] == ')')
	    {
	      /* special namespace provides */
	      if (strcmp(spe, "(<NULL>)") != 0)
		{
		  pieces[1][l - 1] = 0;
		  what = testcase_str2dep(pool, spe + 1);
		  pieces[1][l - 1] = ')';
		}
	      what = pool_rel2id(pool, pool_strn2id(pool, pieces[1], spe - pieces[1], 1), what, REL_NAMESPACE, 1);
	    }
	}
      if (!what)
        what = testcase_str2dep(pool, pieces[1]);
      if (pieces[0][0] == 'n')
	job = SOLVER_SOLVABLE_NAME;
      else
	job = SOLVER_SOLVABLE_PROVIDES;
    }
  else if (!strcmp(pieces[0], "oneof"))
    {
      Queue q;
      job = SOLVER_SOLVABLE_ONE_OF;
      queue_init(&q);
      if (npieces > 1 && strcmp(pieces[1], "nothing") != 0)
	{
	  int i;
	  for (i = 1; i < npieces; i++)
	    {
	      Id p = testcase_str2solvid(pool, pieces[i]);
	      if (!p)
		{
		  queue_free(&q);
		  return pool_error(pool, -1, "%s: unknown package '%s'", caller, pieces[i]);
		}
	      queue_push(&q, p);
	    }
	}
      what = pool_queuetowhatprovides(pool, &q);
      queue_free(&q);
    }
  else if (!strcmp(pieces[0], "repo") && npieces == 2)
    {
      Repo *repo = testcase_str2repo(pool, pieces[1]);
      if (!repo)
	return pool_error(pool, -1, "%s: unknown repo '%s'", caller, pieces[1]);
      job = SOLVER_SOLVABLE_REPO;
      what = repo->repoid;
    }
  else if (!strcmp(pieces[0], "all") && npieces == 2 && !strcmp(pieces[1], "packages"))
    {
      job = SOLVER_SOLVABLE_ALL;
      what = 0;
    }
  else
    {
      /* join em again for the error message... */
      char *sp;
      for (sp = pieces[0]; sp < pieces[npieces - 1]; sp++)
	if (*sp == 0)
	  *sp = ' ';
      return pool_error(pool, -1, "%s: bad line '%s'", caller, pieces[0]);
    }
  *whatp = what;
  return job;
}