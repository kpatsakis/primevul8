onig_regset_replace(OnigRegSet* set, int at, regex_t* reg)
{
  int i;

  if (at < 0 || at >= set->n)
    return ONIGERR_INVALID_ARGUMENT;

  if (IS_NULL(reg)) {
    onig_region_free(set->rs[at].region, 1);
    for (i = at; i < set->n - 1; i++) {
      set->rs[i].reg    = set->rs[i+1].reg;
      set->rs[i].region = set->rs[i+1].region;
    }
    set->n--;
  }
  else {
    if (IS_FIND_LONGEST(reg->options))
      return ONIGERR_INVALID_ARGUMENT;

    if (set->n > 1 && reg->enc != set->enc)
      return ONIGERR_INVALID_ARGUMENT;

    set->rs[at].reg = reg;
  }

  for (i = 0; i < set->n; i++)
    update_regset_by_reg(set, set->rs[i].reg);

  return 0;
}