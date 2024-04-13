mrb_ary_entry(mrb_value ary, mrb_int n)
{
  struct RArray *a = mrb_ary_ptr(ary);
  mrb_int len = ARY_LEN(a);

  /* range check */
  if (n < 0) n += len;
  if (n < 0 || len <= n) return mrb_nil_value();

  return ARY_PTR(a)[n];
}