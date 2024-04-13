free_callout_func_list(CalloutNameListType* s)
{
  if (IS_NOT_NULL(s)) {
    if (IS_NOT_NULL(s->v)) {
      int i, j;

      for (i = 0; i < s->n; i++) {
        CalloutNameListEntry* e = s->v + i;
        for (j = e->arg_num - e->opt_arg_num; j < e->arg_num; j++) {
          if (e->arg_types[j] == ONIG_TYPE_STRING) {
            UChar* p = e->opt_defaults[j].s.start;
            if (IS_NOT_NULL(p)) xfree(p);
          }
        }
      }
      xfree(s->v);
    }
    xfree(s);
  }
}