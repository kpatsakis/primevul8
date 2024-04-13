testcase_str2solvid(Pool *pool, const char *str)
{
  int i, l = strlen(str);
  int repostart;
  Repo *repo;
  Id arch;

  if (!l)
    return 0;
  if (*str == '@' && !strcmp(str, "@SYSTEM"))
    return SYSTEMSOLVABLE;
  repo = 0;
  for (i = l - 1; i >= 0; i--)
    if (str[i] == '@' && (repo = testcase_str2repo(pool, str + i + 1)) != 0)
      break;
  if (i < 0)
    i = l;
  repostart = i;
  /* now find the arch (if present) */
  arch = 0;
  for (i = repostart - 1; i > 0; i--)
    if (str[i] == '.')
      {
	arch = pool_strn2id(pool, str + i + 1, repostart - (i + 1), 0);
	if (arch)
	  repostart = i;
	break;
      }
  /* now find the name */
  for (i = repostart - 1; i > 0; i--)
    {
      if (str[i] == '-')
	{
	  Id nid, evrid, p, pp;
	  nid = pool_strn2id(pool, str, i, 0);
	  if (!nid)
	    continue;
	  evrid = pool_strn2id(pool, str + i + 1, repostart - (i + 1), 0);
	  /* first check whatprovides */
	  FOR_PROVIDES(p, pp, nid)
	    {
	      Solvable *s = pool->solvables + p;
	      if (s->name != nid)
		continue;
	      if (repo && s->repo != repo)
		continue;
	      if (arch && s->arch != arch)
		continue;
	      if (str2solvid_check(pool, s, str + i + 1, str + repostart, evrid))
	        return p;
	    }
	  /* maybe it's not installable and thus not in whatprovides. do a slow search */
	  if (repo)
	    {
	      Solvable *s;
	      FOR_REPO_SOLVABLES(repo, p, s)
		{
		  if (s->name != nid)
		    continue;
		  if (arch && s->arch != arch)
		    continue;
		  if (str2solvid_check(pool, s, str + i + 1, str + repostart, evrid))
		    return p;
		}
	    }
	  else
	    {
	      FOR_POOL_SOLVABLES(p)
		{
		  Solvable *s = pool->solvables + p;
		  if (s->name != nid)
		    continue;
		  if (arch && s->arch != arch)
		    continue;
		  if (str2solvid_check(pool, s, str + i + 1, str + repostart, evrid))
		    return p;
		}
	    }
	}
    }
  return 0;
}