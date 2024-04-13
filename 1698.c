ary_concat(mrb_state *mrb, struct RArray *a, struct RArray *a2)
{
  mrb_int len;

  if (ARY_LEN(a) == 0) {
    ary_replace(mrb, a, a2);
    return;
  }
  if (ARY_LEN(a2) > ARY_MAX_SIZE - ARY_LEN(a)) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "array size too big");
  }
  len = ARY_LEN(a) + ARY_LEN(a2);

  ary_modify(mrb, a);
  if (ARY_CAPA(a) < len) {
    ary_expand_capa(mrb, a, len);
  }
  array_copy(ARY_PTR(a)+ARY_LEN(a), ARY_PTR(a2), ARY_LEN(a2));
  mrb_write_barrier(mrb, (struct RBasic*)a);
  ARY_SET_LEN(a, len);
}