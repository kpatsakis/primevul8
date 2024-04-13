onig_regset_add(OnigRegSet* set, regex_t* reg)
{
  OnigRegion* region;

  if (IS_FIND_LONGEST(reg->options))
    return ONIGERR_INVALID_ARGUMENT;

  if (set->n != 0 && reg->enc != set->enc)
    return ONIGERR_INVALID_ARGUMENT;

  if (set->n >= set->alloc) {
    RR* nrs;
    int new_alloc;

    new_alloc = set->alloc * 2;
    nrs = (RR* )xrealloc(set->rs, sizeof(set->rs[0]) * new_alloc);
    CHECK_NULL_RETURN_MEMERR(nrs);

    set->rs    = nrs;
    set->alloc = new_alloc;
  }

  region = onig_region_new();
  CHECK_NULL_RETURN_MEMERR(region);

  set->rs[set->n].reg    = reg;
  set->rs[set->n].region = region;
  set->n++;

  update_regset_by_reg(set, reg);
  return 0;
}