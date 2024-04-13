mrb_ary_pop(mrb_state *mrb, mrb_value ary)
{
  struct RArray *a = mrb_ary_ptr(ary);
  mrb_int len = ARY_LEN(a);

  ary_modify_check(mrb, a);
  if (len == 0) return mrb_nil_value();
  ARY_SET_LEN(a, len-1);
  return ARY_PTR(a)[len-1];
}