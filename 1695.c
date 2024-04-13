mrb_ary_new_from_values(mrb_state *mrb, mrb_int size, const mrb_value *vals)
{
  struct RArray *a = ary_new_from_values(mrb, size, vals);
  return mrb_obj_value(a);
}