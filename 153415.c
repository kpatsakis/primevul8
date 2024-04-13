reg_callout_list_entry(ScanEnv* env, int* rnum)
{
#define INIT_CALLOUT_LIST_NUM  3

  int num;
  CalloutListEntry* list;
  CalloutListEntry* e;
  RegexExt* ext;

  ext = onig_get_regex_ext(env->reg);
  CHECK_NULL_RETURN_MEMERR(ext);

  if (IS_NULL(ext->callout_list)) {
    list = (CalloutListEntry* )xmalloc(sizeof(*list) * INIT_CALLOUT_LIST_NUM);
    CHECK_NULL_RETURN_MEMERR(list);

    ext->callout_list = list;
    ext->callout_list_alloc = INIT_CALLOUT_LIST_NUM;
    ext->callout_num = 0;
  }

  num = ext->callout_num + 1;
  if (num > ext->callout_list_alloc) {
    int alloc = ext->callout_list_alloc * 2;
    list = (CalloutListEntry* )xrealloc(ext->callout_list,
                                        sizeof(CalloutListEntry) * alloc);
    CHECK_NULL_RETURN_MEMERR(list);

    ext->callout_list       = list;
    ext->callout_list_alloc = alloc;
  }

  e = ext->callout_list + (num - 1);

  e->flag             = 0;
  e->of               = 0;
  e->in               = ONIG_CALLOUT_OF_CONTENTS;
  e->type             = 0;
  e->tag_start        = 0;
  e->tag_end          = 0;
  e->start_func       = 0;
  e->end_func         = 0;
  e->u.arg.num        = 0;
  e->u.arg.passed_num = 0;

  ext->callout_num = num;
  *rnum = num;
  return ONIG_NORMAL;
}