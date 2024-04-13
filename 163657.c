testcase_resultdiff(const char *result1, const char *result2)
{
  Strqueue sq1, sq2, osq;
  char *r;
  strqueue_init(&sq1);
  strqueue_init(&sq2);
  strqueue_init(&osq);
  strqueue_split(&sq1, result1);
  strqueue_split(&sq2, result2);
  strqueue_sort(&sq1);
  strqueue_sort(&sq2);
  strqueue_diff(&sq1, &sq2, &osq);
  r = osq.nstr ? strqueue_join(&osq) : 0;
  strqueue_free(&sq1);
  strqueue_free(&sq2);
  strqueue_free(&osq);
  return r;
}