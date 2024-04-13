static void purge_empty_parameters (PARAMETER **headp)
{
  PARAMETER *p, *q, **last;

  for (last = headp, p = *headp; p; p = q)
  {
    q = p->next;
    if (!p->attribute || !p->value)
    {
      *last = q;
      p->next = NULL;
      mutt_free_parameter (&p);
    }
    else
      last = &p->next;
  }
}