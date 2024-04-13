mrb_ary_svalue(mrb_state *mrb, mrb_value ary)
{
  switch (RARRAY_LEN(ary)) {
  case 0:
    return mrb_nil_value();
  case 1:
    return RARRAY_PTR(ary)[0];
  default:
    return ary;
  }
}