strqueue_sort_u(Strqueue *q)
{
  int i, j;
  strqueue_sort(q);
  for (i = j = 0; i < q->nstr; i++)
    if (!j || strqueue_sort_cmp(q->str + i, q->str + j - 1, 0) != 0)
      q->str[j++] = q->str[i];
  q->nstr = j;
}