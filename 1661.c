mrb_ary_replace(mrb_state *mrb, mrb_value self, mrb_value other)
{
  struct RArray *a1 = mrb_ary_ptr(self);
  struct RArray *a2 = mrb_ary_ptr(other);

  if (a1 != a2) {
    ary_replace(mrb, a1, a2);
  }
}