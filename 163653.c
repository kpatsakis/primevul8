strqueue_join(Strqueue *q)
{
  int i, l = 0;
  char *r, *rp;
  for (i = 0; i < q->nstr; i++)
    if (q->str[i])
      l += strlen(q->str[i]) + 1;
  l++;	/* trailing \0 */
  r = solv_malloc(l);
  rp = r;
  for (i = 0; i < q->nstr; i++)
    if (q->str[i])
      {
        strcpy(rp, q->str[i]);
        rp += strlen(rp);
	*rp++ = '\n';
      }
  *rp = 0;
  return r;
}