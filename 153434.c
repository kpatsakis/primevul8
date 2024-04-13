callout_name_table_clear(CalloutNameTable* t)
{
  int i;
  CalloutNameEntry* e;

  if (IS_NOT_NULL(t)) {
    for (i = 0; i < t->num; i++) {
      e = &(t->e[i]);
      if (IS_NOT_NULL(e->name)) {
        xfree(e->name);
        e->name     = NULL;
        e->name_len = 0;
        e->id       = 0;
        e->func     = 0;
      }
    }
    if (IS_NOT_NULL(t->e)) {
      xfree(t->e);
      t->e = NULL;
    }
    t->num = 0;
  }
  return 0;
}