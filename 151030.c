comp_opt_exact_or_map_info(OptExactInfo* e, OptMapInfo* m)
{
#define COMP_EM_BASE  20
  int ve, vm;

  if (m->value <= 0) return -1;

  ve = COMP_EM_BASE * e->len * (e->ignore_case ? 1 : 2);
  vm = COMP_EM_BASE * 5 * 2 / m->value;
  return comp_distance_value(&e->mmd, &m->mmd, ve, vm);
}