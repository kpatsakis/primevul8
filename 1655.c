ary_subseq(mrb_state *mrb, struct RArray *a, mrb_int beg, mrb_int len)
{
  struct RArray *b;

  if (!ARY_SHARED_P(a) && len <= ARY_SHIFT_SHARED_MIN) {
    return mrb_ary_new_from_values(mrb, len, ARY_PTR(a)+beg);
  }
  ary_make_shared(mrb, a);
  b  = MRB_OBJ_ALLOC(mrb, MRB_TT_ARRAY, mrb->array_class);
  b->as.heap.ptr = a->as.heap.ptr + beg;
  b->as.heap.len = len;
  b->as.heap.aux.shared = a->as.heap.aux.shared;
  b->as.heap.aux.shared->refcnt++;
  ARY_SET_SHARED_FLAG(b);

  return mrb_obj_value(b);
}