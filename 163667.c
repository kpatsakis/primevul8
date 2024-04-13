testcase_str2repo(Pool *pool, const char *str)
{
  int repoid;
  Repo *repo = 0;
  if (str[0] == '#' && (str[1] >= '0' && str[1] <= '9'))
    {
      int j;
      repoid = 0;
      for (j = 1; str[j] >= '0' && str[j] <= '9'; j++)
	repoid = repoid * 10 + (str[j] - '0');
      if (!str[j] && repoid > 0 && repoid < pool->nrepos)
	repo = pool_id2repo(pool, repoid);
    }
  if (!repo)
    {
      FOR_REPOS(repoid, repo)
	{
	  int i, l;
	  if (!repo->name)
	    continue;
	  l = strlen(repo->name);
	  for (i = 0; i < l; i++)
	    {
	      int c = repo->name[i];
	      if (c == ' ' || c == '\t')
		c = '_';
	      if (c != str[i])
		break;
	    }
	  if (i == l && !str[l])
	    break;
	}
      if (repoid >= pool->nrepos)
	repo = 0;
    }
  return repo;
}