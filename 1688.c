mrb_ary_join_m(mrb_state *mrb, mrb_value ary)
{
  mrb_value sep = mrb_nil_value();

  mrb_get_args(mrb, "|S!", &sep);
  return mrb_ary_join(mrb, ary, sep);
}