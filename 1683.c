ary_expand_capa(mrb_state *mrb, struct RArray *a, mrb_int len)
{
  mrb_int capa = ARY_CAPA(a);

  if (len > ARY_MAX_SIZE || len < 0) {
  size_error:
    mrb_raise(mrb, E_ARGUMENT_ERROR, "array size too big");
  }

  if (capa < ARY_DEFAULT_LEN) {
    capa = ARY_DEFAULT_LEN;
  }
  while (capa < len) {
    if (capa <= ARY_MAX_SIZE / 2) {
      capa *= 2;
    }
    else {
      capa = len;
    }
  }
  if (capa < len || capa > ARY_MAX_SIZE) {
    goto size_error;
  }

  if (ARY_EMBED_P(a)) {
    mrb_value *ptr = ARY_EMBED_PTR(a);
    mrb_int len = ARY_EMBED_LEN(a);
    mrb_value *expanded_ptr = (mrb_value *)mrb_malloc(mrb, sizeof(mrb_value)*capa);

    ARY_UNSET_EMBED_FLAG(a);
    array_copy(expanded_ptr, ptr, len);
    a->as.heap.len = len;
    a->as.heap.aux.capa = capa;
    a->as.heap.ptr = expanded_ptr;
  }
  else if (capa > a->as.heap.aux.capa) {
    mrb_value *expanded_ptr = (mrb_value *)mrb_realloc(mrb, a->as.heap.ptr, sizeof(mrb_value)*capa);

    a->as.heap.aux.capa = capa;
    a->as.heap.ptr = expanded_ptr;
  }
}