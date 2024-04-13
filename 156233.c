onig_regset_free(OnigRegSet* set)
{
  int i;

  for (i = 0; i < set->n; i++) {
    regex_t* reg;
    OnigRegion* region;

    reg    = set->rs[i].reg;
    region = set->rs[i].region;
    onig_free(reg);
    if (IS_NOT_NULL(region))
      onig_region_free(region, 1);
  }

  xfree(set->rs);
  xfree(set);
}