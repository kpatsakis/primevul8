ary_new_capa(mrb_state *mrb, mrb_int capa)
{
  struct RArray *a;
  size_t blen;

  if (capa > ARY_MAX_SIZE) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "array size too big");
  }
  blen = capa * sizeof(mrb_value);

  a = MRB_OBJ_ALLOC(mrb, MRB_TT_ARRAY, mrb->array_class);
  if (capa <= MRB_ARY_EMBED_LEN_MAX) {
    ARY_SET_EMBED_LEN(a, 0);
  }
  else {
    a->as.heap.ptr = (mrb_value *)mrb_malloc(mrb, blen);
    a->as.heap.aux.capa = capa;
    a->as.heap.len = 0;
  }

  return a;
}