mrb_ary_aset(mrb_state *mrb, mrb_value self)
{
  mrb_value v1, v2, v3;
  mrb_int i, len;

  ary_modify(mrb, mrb_ary_ptr(self));
  if (mrb_get_argc(mrb) == 2) {
    const mrb_value *vs = mrb_get_argv(mrb);
    v1 = vs[0]; v2 = vs[1];

    /* a[n..m] = v */
    switch (mrb_range_beg_len(mrb, v1, &i, &len, RARRAY_LEN(self), FALSE)) {
    case MRB_RANGE_TYPE_MISMATCH:
      mrb_ary_set(mrb, self, aget_index(mrb, v1), v2);
      break;
    case MRB_RANGE_OK:
      mrb_ary_splice(mrb, self, i, len, v2);
      break;
    case MRB_RANGE_OUT:
      mrb_raisef(mrb, E_RANGE_ERROR, "%v out of range", v1);
      break;
    }
    return v2;
  }

  mrb_get_args(mrb, "ooo", &v1, &v2, &v3);
  /* a[n,m] = v */
  mrb_ary_splice(mrb, self, aget_index(mrb, v1), aget_index(mrb, v2), v3);
  return v3;
}