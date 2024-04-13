mrb_assoc_new(mrb_state *mrb, mrb_value car, mrb_value cdr)
{
  struct RArray *a;

  a = ary_new_capa(mrb, 2);
  ARY_PTR(a)[0] = car;
  ARY_PTR(a)[1] = cdr;
  ARY_SET_LEN(a, 2);
  return mrb_obj_value(a);
}