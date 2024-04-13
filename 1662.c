mrb_ary_splat(mrb_state *mrb, mrb_value v)
{
  mrb_value ary;
  struct RArray *a;

  if (mrb_array_p(v)) {
    a = ary_dup(mrb, mrb_ary_ptr(v));
    return mrb_obj_value(a);
  }

  if (!mrb_respond_to(mrb, v, MRB_SYM(to_a))) {
    return mrb_ary_new_from_values(mrb, 1, &v);
  }

  ary = mrb_funcall_id(mrb, v, MRB_SYM(to_a), 0);
  if (mrb_nil_p(ary)) {
    return mrb_ary_new_from_values(mrb, 1, &v);
  }
  mrb_ensure_array_type(mrb, ary);
  a = mrb_ary_ptr(ary);
  a = ary_dup(mrb, a);
  return mrb_obj_value(a);
}