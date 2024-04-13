onig_foreach_name(regex_t* reg,
  int (*func)(const UChar*, const UChar*,int,int*,regex_t*,void*), void* arg)
{
  int i, r;
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t)) {
    for (i = 0; i < t->num; i++) {
      e = &(t->e[i]);
      r = (*func)(e->name, e->name + e->name_len, e->back_num,
		  (e->back_num > 1 ? e->back_refs : &(e->back_ref1)),
		  reg, arg);
      if (r != 0) return r;
    }
  }
  return 0;
}