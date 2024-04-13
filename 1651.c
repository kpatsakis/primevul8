mrb_ary_aget(mrb_state *mrb, mrb_value self)
{
  struct RArray *a = mrb_ary_ptr(self);
  mrb_int i;
  mrb_int len, alen;
  mrb_value index;

  if (mrb_get_argc(mrb) == 1) {
    index = mrb_get_arg1(mrb);
    switch (mrb_type(index)) {
      /* a[n..m] */
    case MRB_TT_RANGE:
      if (mrb_range_beg_len(mrb, index, &i, &len, ARY_LEN(a), TRUE) == MRB_RANGE_OK) {
        return ary_subseq(mrb, a, i, len);
      }
      else {
        return mrb_nil_value();
      }
    case MRB_TT_INTEGER:
      return mrb_ary_ref(mrb, self, mrb_integer(index));
    default:
      return mrb_ary_ref(mrb, self, aget_index(mrb, index));
    }
  }

  mrb_get_args(mrb, "oi", &index, &len);
  i = aget_index(mrb, index);
  alen = ARY_LEN(a);
  if (i < 0) i += alen;
  if (i < 0 || alen < i) return mrb_nil_value();
  if (len < 0) return mrb_nil_value();
  if (alen == i) return mrb_ary_new(mrb);
  if (len > alen - i) len = alen - i;

  return ary_subseq(mrb, a, i, len);
}