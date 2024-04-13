strqueue_pushjoin(Strqueue *q, const char *s1, const char *s2, const char *s3)
{
  q->str = solv_extend(q->str, q->nstr, 1, sizeof(*q->str), STRQUEUE_BLOCK);
  q->str[q->nstr++] = solv_dupjoin(s1, s2, s3);
}