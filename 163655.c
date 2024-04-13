testcase_ruleid(Solver *solv, Id rid)
{
  Strqueue sq;
  Queue q;
  int i;
  Chksum *chk;
  const unsigned char *md5;
  int md5l;
  const char *s;

  queue_init(&q);
  strqueue_init(&sq);
  solver_ruleliterals(solv, rid, &q);
  for (i = 0; i < q.count; i++)
    {
      Id p = q.elements[i];
      s = testcase_solvid2str(solv->pool, p > 0 ? p : -p);
      if (p < 0)
	s = pool_tmpjoin(solv->pool, "!", s, 0);
      strqueue_push(&sq, s);
    }
  queue_free(&q);
  strqueue_sort_u(&sq);
  chk = solv_chksum_create(REPOKEY_TYPE_MD5);
  for (i = 0; i < sq.nstr; i++)
    solv_chksum_add(chk, sq.str[i], strlen(sq.str[i]) + 1);
  md5 = solv_chksum_get(chk, &md5l);
  s = pool_bin2hex(solv->pool, md5, md5l);
  chk = solv_chksum_free(chk, 0);
  strqueue_free(&sq);
  return s;
}