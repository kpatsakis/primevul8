mrb_ary_replace_m(mrb_state *mrb, mrb_value self)
{
  mrb_value other;

  mrb_get_args(mrb, "A", &other);
  mrb_ary_replace(mrb, self, other);

  return self;
}