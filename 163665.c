testcase_problemid(Solver *solv, Id problem)
{
  Strqueue sq;
  Queue q;
  Chksum *chk;
  const unsigned char *md5;
  int i, md5l;
  const char *s;

  /* we build a hash of all rules that define the problem */
  queue_init(&q);
  strqueue_init(&sq);
  solver_findallproblemrules(solv, problem, &q);
  for (i = 0; i < q.count; i++)
    strqueue_push(&sq, testcase_ruleid(solv, q.elements[i]));
  queue_free(&q);
  strqueue_sort_u(&sq);
  chk = solv_chksum_create(REPOKEY_TYPE_MD5);
  for (i = 0; i < sq.nstr; i++)
    solv_chksum_add(chk, sq.str[i], strlen(sq.str[i]) + 1);
  md5 = solv_chksum_get(chk, &md5l);
  s = pool_bin2hex(solv->pool, md5, 4);
  chk = solv_chksum_free(chk, 0);
  strqueue_free(&sq);
  return s;
}