mrb_ary_delete_at(mrb_state *mrb, mrb_value self)
{
  struct RArray *a = mrb_ary_ptr(self);
  mrb_int   index;
  mrb_value val;
  mrb_value *ptr;
  mrb_int len, alen;

  mrb_get_args(mrb, "i", &index);
  alen = ARY_LEN(a);
  if (index < 0) index += alen;
  if (index < 0 || alen <= index) return mrb_nil_value();

  ary_modify(mrb, a);
  ptr = ARY_PTR(a);
  val = ptr[index];

  ptr += index;
  len = alen - index;
  while (--len) {
    *ptr = *(ptr+1);
    ++ptr;
  }
  ARY_SET_LEN(a, alen-1);

  ary_shrink_capa(mrb, a);

  return val;
}