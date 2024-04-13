mrb_ary_first(mrb_state *mrb, mrb_value self)
{
  struct RArray *a = mrb_ary_ptr(self);
  mrb_int size, alen;

  if (mrb_get_argc(mrb) == 0) {
    return (ARY_LEN(a) > 0)? ARY_PTR(a)[0]: mrb_nil_value();
  }
  mrb_get_args(mrb, "|i", &size);
  if (size < 0) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "negative array size");
  }

  alen = ARY_LEN(a);
  if (size > alen) size = alen;
  if (ARY_SHARED_P(a)) {
    return ary_subseq(mrb, a, 0, size);
  }
  return mrb_ary_new_from_values(mrb, size, ARY_PTR(a));
}