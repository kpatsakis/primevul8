mrb_ary_push_m(mrb_state *mrb, mrb_value self)
{
  mrb_int argc;
  const mrb_value *argv;
  mrb_int len, len2;
  struct RArray *a;

  argc = mrb_get_argc(mrb);
  argv = mrb_get_argv(mrb);
  a = mrb_ary_ptr(self);
  ary_modify(mrb, a);
  len = ARY_LEN(a);
  len2 = len + argc;
  if (ARY_CAPA(a) < len2) {
    ary_expand_capa(mrb, a, len2);
  }
  array_copy(ARY_PTR(a)+len, argv, argc);
  ARY_SET_LEN(a, len2);
  while (argc--) {
    mrb_field_write_barrier_value(mrb, (struct RBasic*)a, *argv);
    argv++;
  }
  return self;
}