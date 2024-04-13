static void rfc2231_list_insert (struct rfc2231_parameter **list,
				 struct rfc2231_parameter *par)
{
  struct rfc2231_parameter **last = list;
  struct rfc2231_parameter *p = *list;
  int c;

  while (p)
  {
    c = strcmp (par->attribute, p->attribute);
    if ((c < 0) || (c == 0 && par->index <= p->index))
      break;

    last = &p->next;
    p = p->next;
  }

  par->next = p;
  *last = par;
}