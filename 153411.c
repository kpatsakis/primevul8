onig_free_reg_callout_list(int n, CalloutListEntry* list)
{
  int i;
  int j;

  if (IS_NULL(list)) return ;

  for (i = 0; i < n; i++) {
    if (list[i].of == ONIG_CALLOUT_OF_NAME) {
      for (j = 0; j < list[i].u.arg.passed_num; j++) {
        if (list[i].u.arg.types[j] == ONIG_TYPE_STRING) {
          if (IS_NOT_NULL(list[i].u.arg.vals[j].s.start))
            xfree(list[i].u.arg.vals[j].s.start);
        }
      }
    }
    else { /* ONIG_CALLOUT_OF_CONTENTS */
      if (IS_NOT_NULL(list[i].u.content.start)) {
        xfree((void* )list[i].u.content.start);
      }
    }
  }

  xfree(list);
}