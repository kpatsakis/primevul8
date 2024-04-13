strqueue_push(Strqueue *q, const char *s)
{
  q->str = solv_extend(q->str, q->nstr, 1, sizeof(*q->str), STRQUEUE_BLOCK);
  q->str[q->nstr++] = solv_strdup(s);
}