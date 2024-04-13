swap_repo_names(Pool *pool, const char **names)
{
  int repoid;
  Repo *repo;
  FOR_REPOS(repoid, repo)
    {
      const char *n = repo->name;
      repo->name = names[repoid];
      names[repoid] = n;
    }
}