testcase_alternativeid(Solver *solv, int type, Id id, Id from)
{
  const char *s;
  Pool *pool = solv->pool;
  Chksum *chk;
  const unsigned char *md5;
  int md5l;
  chk = solv_chksum_create(REPOKEY_TYPE_MD5);
  if (type == SOLVER_ALTERNATIVE_TYPE_RECOMMENDS)
    {
      s = testcase_solvid2str(pool, from);
      solv_chksum_add(chk, s, strlen(s) + 1);
      s = testcase_dep2str(pool, id);
      solv_chksum_add(chk, s, strlen(s) + 1);
    }
  else if (type == SOLVER_ALTERNATIVE_TYPE_RULE)
    {
      s = testcase_ruleid(solv, id);
      solv_chksum_add(chk, s, strlen(s) + 1);
    }
  md5 = solv_chksum_get(chk, &md5l);
  s = pool_bin2hex(pool, md5, 4);
  chk = solv_chksum_free(chk, 0);
  return s;
}