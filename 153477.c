callout_func_list_add(CalloutNameListType* s, int* rid)
{
  if (s->n >= s->alloc) {
    int new_size = s->alloc * 2;
    CalloutNameListEntry* nv = (CalloutNameListEntry* )
      xrealloc(s->v, sizeof(CalloutNameListEntry) * new_size);
    if (IS_NULL(nv)) return ONIGERR_MEMORY;

    s->alloc = new_size;
    s->v = nv;
  }

  *rid = s->n;

  xmemset(&(s->v[s->n]), 0, sizeof(*(s->v)));
  s->n++;
  return ONIG_NORMAL;
}