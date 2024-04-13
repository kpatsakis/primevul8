mrb_ary_index_m(mrb_state *mrb, mrb_value self)
{
  mrb_value obj = mrb_get_arg1(mrb);
  mrb_int i;

  for (i = 0; i < RARRAY_LEN(self); i++) {
    if (mrb_equal(mrb, RARRAY_PTR(self)[i], obj)) {
      return mrb_int_value(mrb, i);
    }
  }
  return mrb_nil_value();
}