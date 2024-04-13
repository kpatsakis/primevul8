mrb_ary_concat(mrb_state *mrb, mrb_value self, mrb_value other)
{
  struct RArray *a2 = mrb_ary_ptr(other);

  ary_concat(mrb, mrb_ary_ptr(self), a2);
}