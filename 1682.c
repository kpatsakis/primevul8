mrb_ary_cmp(mrb_state *mrb, mrb_value ary1)
{
  mrb_value ary2 = mrb_get_arg1(mrb);

  mrb->c->ci->mid = 0;
  if (mrb_obj_equal(mrb, ary1, ary2)) return mrb_fixnum_value(0);
  if (!mrb_array_p(ary2)) {
    return mrb_nil_value();
  }

  return ary2;
}