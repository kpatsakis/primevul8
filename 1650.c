aget_index(mrb_state *mrb, mrb_value index)
{
  if (mrb_integer_p(index)) {
    return mrb_integer(index);
  }
#ifndef MRB_NO_FLOAT
  else if (mrb_float_p(index)) {
    return (mrb_int)mrb_float(index);
  }
#endif
  else {
    mrb_int i, argc;
    const mrb_value *argv;

    mrb_get_args(mrb, "i*!", &i, &argv, &argc);
    return i;
  }
}