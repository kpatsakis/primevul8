mrb_ary_last(mrb_state *mrb, mrb_value self)
{
  struct RArray *a = mrb_ary_ptr(self);
  mrb_int n, size, alen;

  n = mrb_get_args(mrb, "|i", &size);
  alen = ARY_LEN(a);
  if (n == 0) {
    return (alen > 0) ? ARY_PTR(a)[alen - 1]: mrb_nil_value();
  }

  if (size < 0) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "negative array size");
  }
  if (size > alen) size = alen;
  if (ARY_SHARED_P(a) || size > ARY_DEFAULT_LEN) {
    return ary_subseq(mrb, a, alen - size, size);
  }
  return mrb_ary_new_from_values(mrb, size, ARY_PTR(a) + alen - size);
}