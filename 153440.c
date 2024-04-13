make_callout_func_list(CalloutNameListType** rs, int init_size)
{
  CalloutNameListType* s;
  CalloutNameListEntry* v;

  *rs = 0;

  s = xmalloc(sizeof(*s));
  if (IS_NULL(s)) return ONIGERR_MEMORY;

  v = (CalloutNameListEntry* )xmalloc(sizeof(CalloutNameListEntry) * init_size);
  if (IS_NULL(v)) {
    xfree(s);
    return ONIGERR_MEMORY;
  }

  s->n = 0;
  s->alloc = init_size;
  s->v = v;

  *rs = s;
  return ONIG_NORMAL;
}