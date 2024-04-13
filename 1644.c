mrb_ary_clear(mrb_state *mrb, mrb_value self)
{
  struct RArray *a = mrb_ary_ptr(self);

  ary_modify(mrb, a);
  if (ARY_SHARED_P(a)) {
    mrb_ary_decref(mrb, a->as.heap.aux.shared);
    ARY_UNSET_SHARED_FLAG(a);
  }
  else if (!ARY_EMBED_P(a)){
    mrb_free(mrb, a->as.heap.ptr);
  }
  if (MRB_ARY_EMBED_LEN_MAX > 0) {
    ARY_SET_EMBED_LEN(a, 0);
  }
  else {
    a->as.heap.ptr = NULL;
    a->as.heap.aux.capa = 0;
    ARY_SET_LEN(a, 0);
  }
  return self;
}