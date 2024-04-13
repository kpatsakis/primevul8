mrb_ary_unshift(mrb_state *mrb, mrb_value self, mrb_value item)
{
  struct RArray *a = mrb_ary_ptr(self);
  mrb_int len = ARY_LEN(a);

  if (ARY_SHARED_P(a)
      && a->as.heap.aux.shared->refcnt == 1 /* shared only referenced from this array */
      && a->as.heap.ptr - a->as.heap.aux.shared->ptr >= 1) /* there's room for unshifted item */ {
    a->as.heap.ptr--;
    a->as.heap.ptr[0] = item;
  }
  else {
    mrb_value *ptr;

    ary_modify(mrb, a);
    if (ARY_CAPA(a) < len + 1)
      ary_expand_capa(mrb, a, len + 1);
    ptr = ARY_PTR(a);
    value_move(ptr + 1, ptr, len);
    ptr[0] = item;
  }
  ARY_SET_LEN(a, len+1);
  mrb_field_write_barrier_value(mrb, (struct RBasic*)a, item);

  return self;
}