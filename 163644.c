testcase_mangle_repo_names(Pool *pool)
{
  int i, repoid, mangle = 1;
  Repo *repo;
  const char **names = solv_calloc(pool->nrepos, sizeof(char *));
  FOR_REPOS(repoid, repo)
    {
      char *buf, *mp;
      buf = solv_malloc((repo->name ? strlen(repo->name) : 0) + 40);
      if (!repo->name || !repo->name[0])
        sprintf(buf, "#%d", repoid);
      else
	strcpy(buf, repo->name);
      for (mp = buf; *mp; mp++)
	if (*mp == ' ' || *mp == '\t' || *mp == '/')
	  *mp = '_';
      for (i = 1; i < repoid; i++)
        {
	  if (!names[i] || strcmp(buf, names[i]) != 0)
	    continue;
          sprintf(mp, "_%d", mangle++);
	  i = 0;	/* restart conflict check */
	}
      names[repoid] = buf;
    }
  return names;
}