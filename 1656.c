mrb_ary_s_create(mrb_state *mrb, mrb_value klass)
{
  mrb_value ary;
  const mrb_value *vals;
  mrb_int len;
  struct RArray *a;

  mrb_get_args(mrb, "*!", &vals, &len);
  ary = mrb_ary_new_from_values(mrb, len, vals);
  a = mrb_ary_ptr(ary);
  a->c = mrb_class_ptr(klass);

  return ary;
}