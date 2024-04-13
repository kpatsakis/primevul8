onig_regset_new(OnigRegSet** rset, int n, regex_t* regs[])
{
#define REGSET_INITIAL_ALLOC_SIZE   10

  int i;
  int r;
  int alloc;
  OnigRegSet* set;
  RR* rs;

  *rset = 0;

  set = (OnigRegSet* )xmalloc(sizeof(*set));
  CHECK_NULL_RETURN_MEMERR(set);

  alloc = n > REGSET_INITIAL_ALLOC_SIZE ? n : REGSET_INITIAL_ALLOC_SIZE;
  rs = (RR* )xmalloc(sizeof(set->rs[0]) * alloc);
  if (IS_NULL(rs)) {
    xfree(set);
    return ONIGERR_MEMORY;
  }

  set->rs    = rs;
  set->n     = 0;
  set->alloc = alloc;

  for (i = 0; i < n; i++) {
    regex_t* reg = regs[i];

    r = onig_regset_add(set, reg);
    if (r != 0) {
      for (i = 0; i < set->n; i++) {
        OnigRegion* region = set->rs[i].region;
        if (IS_NOT_NULL(region))
          onig_region_free(region, 1);
      }
      xfree(set->rs);
      xfree(set);
      return r;
    }
  }

  *rset = set;
  return 0;
}