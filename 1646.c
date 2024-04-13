mrb_ary_rindex_m(mrb_state *mrb, mrb_value self)
{
  mrb_value obj = mrb_get_arg1(mrb);
  mrb_int i, len;

  for (i = RARRAY_LEN(self) - 1; i >= 0; i--) {
    if (mrb_equal(mrb, RARRAY_PTR(self)[i], obj)) {
      return mrb_int_value(mrb, i);
    }
    if (i > (len = RARRAY_LEN(self))) {
      i = len;
    }
  }
  return mrb_nil_value();
}