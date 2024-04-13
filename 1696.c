mrb_ary_modify(mrb_state *mrb, struct RArray* a)
{
  mrb_write_barrier(mrb, (struct RBasic*)a);
  ary_modify(mrb, a);
}