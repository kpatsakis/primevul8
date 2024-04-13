mrb_ary_resize(mrb_state *mrb, mrb_value ary, mrb_int new_len)
{
  mrb_int old_len;
  struct RArray *a = mrb_ary_ptr(ary);

  ary_modify(mrb, a);
  old_len = RARRAY_LEN(ary);
  if (old_len != new_len) {
    if (new_len < old_len) {
      ary_shrink_capa(mrb, a);
    }
    else {
      ary_expand_capa(mrb, a, new_len);
      ary_fill_with_nil(ARY_PTR(a) + old_len, new_len - old_len);
    }
    ARY_SET_LEN(a, new_len);
  }

  return ary;
}