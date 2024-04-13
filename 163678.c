testcase_solutionid(Solver *solv, Id problem, Id solution)
{
  Id intid;
  Chksum *chk;
  const unsigned char *md5;
  int md5l;
  const char *s;

  intid = solver_solutionelement_internalid(solv, problem, solution);
  /* internal stuff! handle with care! */
  if (intid < 0)
    {
      /* it's a job */
      s = testcase_job2str(solv->pool, solv->job.elements[-intid - 1], solv->job.elements[-intid]);
    }
  else
    {
      /* it's a rule */
      s = testcase_ruleid(solv, intid);
    }
  chk = solv_chksum_create(REPOKEY_TYPE_MD5);
  solv_chksum_add(chk, s, strlen(s) + 1);
  md5 = solv_chksum_get(chk, &md5l);
  s = pool_bin2hex(solv->pool, md5, 4);
  chk = solv_chksum_free(chk, 0);
  return s;
}