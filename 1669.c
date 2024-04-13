mrb_ary_empty_p(mrb_state *mrb, mrb_value self)
{
  struct RArray *a = mrb_ary_ptr(self);

  return mrb_bool_value(ARY_LEN(a) == 0);
}