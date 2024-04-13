mrb_ary_unshift_m(mrb_state *mrb, mrb_value self)
{
  struct RArray *a = mrb_ary_ptr(self);
  const mrb_value *vals;
  mrb_value *ptr;
  mrb_int alen, len;

  mrb_get_args(mrb, "*!", &vals, &alen);
  if (alen == 0) {
    ary_modify_check(mrb, a);
    return self;
  }
  len = ARY_LEN(a);
  if (alen > ARY_MAX_SIZE - len) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "array size too big");
  }
  if (ARY_SHARED_P(a)
      && a->as.heap.aux.shared->refcnt == 1 /* shared only referenced from this array */
      && a->as.heap.ptr - a->as.heap.aux.shared->ptr >= alen) /* there's room for unshifted item */ {
    ary_modify_check(mrb, a);
    a->as.heap.ptr -= alen;
    ptr = a->as.heap.ptr;
  }
  else {
    mrb_bool same = vals == ARY_PTR(a);
    ary_modify(mrb, a);
    if (ARY_CAPA(a) < len + alen)
      ary_expand_capa(mrb, a, len + alen);
    ptr = ARY_PTR(a);
    value_move(ptr + alen, ptr, len);
    if (same) vals = ptr;
  }
  array_copy(ptr, vals, alen);
  ARY_SET_LEN(a, len+alen);
  while (alen--) {
    mrb_field_write_barrier_value(mrb, (struct RBasic*)a, vals[alen]);
  }

  return self;
}