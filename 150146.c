select_opt_map_info(OptMapInfo* now, OptMapInfo* alt)
{
  const int z = 1<<15; /* 32768: something big value */

  int v1, v2;

  if (alt->value == 0) return ;
  if (now->value == 0) {
    copy_opt_map_info(now, alt);
    return ;
  }

  v1 = z / now->value;
  v2 = z / alt->value;
  if (comp_distance_value(&now->mmd, &alt->mmd, v1, v2) > 0)
    copy_opt_map_info(now, alt);
}