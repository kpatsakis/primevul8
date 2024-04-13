strqueue_split(Strqueue *q, const char *s)
{
  const char *p;
  if (!s)
    return;
  while ((p = strchr(s, '\n')) != 0)
    {
      q->str = solv_extend(q->str, q->nstr, 1, sizeof(*q->str), STRQUEUE_BLOCK);
      q->str[q->nstr] = solv_malloc(p - s + 1);
      if (p > s)
	memcpy(q->str[q->nstr], s, p - s);
      q->str[q->nstr][p - s] = 0;
      q->nstr++;
      s = p + 1;
    }
  if (*s)
    strqueue_push(q, s);
}