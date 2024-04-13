join_ary(mrb_state *mrb, mrb_value ary, mrb_value sep, mrb_value list)
{
  mrb_int i;
  mrb_value result, val, tmp;

  /* check recursive */
  for (i=0; i<RARRAY_LEN(list); i++) {
    if (mrb_obj_equal(mrb, ary, RARRAY_PTR(list)[i])) {
      mrb_raise(mrb, E_ARGUMENT_ERROR, "recursive array join");
    }
  }

  mrb_ary_push(mrb, list, ary);

  result = mrb_str_new_capa(mrb, 64);

  for (i=0; i<RARRAY_LEN(ary); i++) {
    if (i > 0 && !mrb_nil_p(sep)) {
      mrb_str_cat_str(mrb, result, sep);
    }

    val = RARRAY_PTR(ary)[i];
    switch (mrb_type(val)) {
    case MRB_TT_ARRAY:
    ary_join:
      val = join_ary(mrb, val, sep, list);
      /* fall through */

    case MRB_TT_STRING:
    str_join:
      mrb_str_cat_str(mrb, result, val);
      break;

    default:
      if (!mrb_immediate_p(val)) {
        tmp = mrb_check_string_type(mrb, val);
        if (!mrb_nil_p(tmp)) {
          val = tmp;
          goto str_join;
        }
        tmp = mrb_check_array_type(mrb, val);
        if (!mrb_nil_p(tmp)) {
          val = tmp;
          goto ary_join;
        }
      }
      val = mrb_obj_as_string(mrb, val);
      goto str_join;
    }
  }

  mrb_ary_pop(mrb, list);

  return result;
}