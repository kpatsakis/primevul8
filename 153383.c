callout_name_find(UChar* name, UChar* name_end)
{
  int i, len;
  CalloutNameEntry* e;
  CalloutNameTable* t = Calloutnames;

  if (IS_NOT_NULL(t)) {
    len = name_end - name;
    for (i = 0; i < t->num; i++) {
      e = &(t->e[i]);
      if (len == e->name_len && onig_strncmp(name, e->name, len) == 0)
        return e;
    }
  }
  return (CalloutNameEntry* )NULL;
}