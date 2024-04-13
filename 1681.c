ary_modify_check(mrb_state *mrb, struct RArray *a)
{
  mrb_check_frozen(mrb, a);
}