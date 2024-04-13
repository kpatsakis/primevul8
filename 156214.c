onig_regset_get_region(OnigRegSet* set, int at)
{
  if (at < 0 || at >= set->n)
    return (OnigRegion* )0;

  return set->rs[at].region;
}