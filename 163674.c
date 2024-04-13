testcase_solvid2str(Pool *pool, Id p)
{
  Solvable *s = pool->solvables + p;
  const char *n, *e, *a;
  char *str, buf[20];

  if (p == SYSTEMSOLVABLE)
    return "@SYSTEM";
  n = pool_id2str(pool, s->name);
  e = pool_id2str(pool, s->evr);
  a = pool_id2str(pool, s->arch);
  str = pool_alloctmpspace(pool, strlen(n) + strlen(e) + strlen(a) + 3);
  sprintf(str, "%s-%s", n, e);
  if (solvable_lookup_type(s, SOLVABLE_BUILDFLAVOR))
    {
      Queue flavorq;
      int i;

      queue_init(&flavorq);
      solvable_lookup_idarray(s, SOLVABLE_BUILDFLAVOR, &flavorq);
      for (i = 0; i < flavorq.count; i++)
	str = pool_tmpappend(pool, str, "-", pool_id2str(pool, flavorq.elements[i]));
      queue_free(&flavorq);
    }
  if (s->arch)
    str = pool_tmpappend(pool, str, ".", a);
  if (!s->repo)
    return pool_tmpappend(pool, str, "@", 0);
  if (s->repo->name)
    {
      int l = strlen(str);
      str = pool_tmpappend(pool, str, "@", s->repo->name);
      for (; str[l]; l++)
	if (str[l] == ' ' || str[l] == '\t')
	  str[l] = '_';
      return str;
    }
  sprintf(buf, "@#%d", s->repo->repoid);
  return pool_tmpappend(pool, str, buf, 0);
}