select_opt_exact_info(OnigEncoding enc, OptExactInfo* now, OptExactInfo* alt)
{
  int v1, v2;

  v1 = now->len;
  v2 = alt->len;

  if (v2 == 0) {
    return ;
  }
  else if (v1 == 0) {
    copy_opt_exact_info(now, alt);
    return ;
  }
  else if (v1 <= 2 && v2 <= 2) {
    /* ByteValTable[x] is big value --> low price */
    v2 = map_position_value(enc, now->s[0]);
    v1 = map_position_value(enc, alt->s[0]);

    if (now->len > 1) v1 += 5;
    if (alt->len > 1) v2 += 5;
  }

  if (now->ignore_case == 0) v1 *= 2;
  if (alt->ignore_case == 0) v2 *= 2;

  if (comp_distance_value(&now->mmd, &alt->mmd, v1, v2) > 0)
    copy_opt_exact_info(now, alt);
}