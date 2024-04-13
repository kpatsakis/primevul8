strqueue_free(Strqueue *q)
{
  int i;
  for (i = 0; i < q->nstr; i++)
    solv_free(q->str[i]);
  q->str = solv_free(q->str);
  q->nstr = 0;
}