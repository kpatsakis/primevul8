mrb_ary_eq(mrb_state *mrb, mrb_value ary1)
{
  mrb_value ary2 = mrb_get_arg1(mrb);

  mrb->c->ci->mid = 0;
  if (mrb_obj_equal(mrb, ary1, ary2)) return mrb_true_value();
  if (!mrb_array_p(ary2)) {
    return mrb_false_value();
  }
  if (RARRAY_LEN(ary1) != RARRAY_LEN(ary2)) return mrb_false_value();

  return ary2;
}