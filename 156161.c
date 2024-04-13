adjust_match_param(regex_t* reg, OnigMatchParam* mp)
{
  RegexExt* ext = reg->extp;

  mp->match_at_call_counter = 0;

  if (IS_NULL(ext) || ext->callout_num == 0) return ONIG_NORMAL;

  if (ext->callout_num > mp->callout_data_alloc_num) {
    CalloutData* d;
    size_t n = ext->callout_num * sizeof(*d);
    if (IS_NOT_NULL(mp->callout_data))
      d = (CalloutData* )xrealloc(mp->callout_data, n);
    else
      d = (CalloutData* )xmalloc(n);
    CHECK_NULL_RETURN_MEMERR(d);

    mp->callout_data = d;
    mp->callout_data_alloc_num = ext->callout_num;
  }

  xmemset(mp->callout_data, 0, mp->callout_data_alloc_num * sizeof(CalloutData));
  return ONIG_NORMAL;
}