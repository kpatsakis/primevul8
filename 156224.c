onig_regset_get_regex(OnigRegSet* set, int at)
{
  if (at < 0 || at >= set->n)
    return (regex_t* )0;

  return set->rs[at].reg;
}