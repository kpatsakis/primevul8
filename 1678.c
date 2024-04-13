mrb_ary_size(mrb_state *mrb, mrb_value self)
{
  struct RArray *a = mrb_ary_ptr(self);

  return mrb_int_value(mrb, ARY_LEN(a));
}