onig_builtin_cmp(OnigCalloutArgs* args, void* user_data ARG_UNUSED)
{
  int r;
  int slot;
  long lv;
  long rv;
  OnigType  type;
  OnigValue val;
  regex_t* reg;
  enum OP_CMP op;

  reg = args->regex;

  r = onig_get_arg_by_callout_args(args, 0, &type, &val);
  if (r != ONIG_NORMAL) return r;

  if (type == ONIG_TYPE_TAG) {
    r = onig_get_callout_data_by_callout_args(args, val.tag, 0, &type, &val);
    if (r < ONIG_NORMAL) return r;
    else if (r > ONIG_NORMAL)
      lv = 0L;
    else
      lv = val.l;
  }
  else { /* ONIG_TYPE_LONG */
    lv = val.l;
  }

  r = onig_get_arg_by_callout_args(args, 2, &type, &val);
  if (r != ONIG_NORMAL) return r;

  if (type == ONIG_TYPE_TAG) {
    r = onig_get_callout_data_by_callout_args(args, val.tag, 0, &type, &val);
    if (r < ONIG_NORMAL) return r;
    else if (r > ONIG_NORMAL)
      rv = 0L;
    else
      rv = val.l;
  }
  else { /* ONIG_TYPE_LONG */
    rv = val.l;
  }

  slot = 0;
  r = onig_get_callout_data_by_callout_args_self(args, slot, &type, &val);
  if (r < ONIG_NORMAL)
    return r;
  else if (r > ONIG_NORMAL) {
    /* type == void: initial state */
    OnigCodePoint c1, c2;
    UChar* p;

    r = onig_get_arg_by_callout_args(args, 1, &type, &val);
    if (r != ONIG_NORMAL) return r;

    p = val.s.start;
    c1 = ONIGENC_MBC_TO_CODE(reg->enc, p, val.s.end);
    p += ONIGENC_MBC_ENC_LEN(reg->enc, p);
    if (p < val.s.end) {
      c2 = ONIGENC_MBC_TO_CODE(reg->enc, p, val.s.end);
      p += ONIGENC_MBC_ENC_LEN(reg->enc, p);
      if (p != val.s.end)  return ONIGERR_INVALID_CALLOUT_ARG;
    }
    else
      c2 = 0;

    switch (c1) {
    case '=':
      if (c2 != '=') return ONIGERR_INVALID_CALLOUT_ARG;
      op = OP_EQ;
      break;
    case '!':
      if (c2 != '=') return ONIGERR_INVALID_CALLOUT_ARG;
      op = OP_NE;
      break;
    case '<':
      if (c2 == '=') op = OP_LE;
      else if (c2 == 0) op = OP_LT;
      else  return ONIGERR_INVALID_CALLOUT_ARG;
      break;
    case '>':
      if (c2 == '=') op = OP_GE;
      else if (c2 == 0) op = OP_GT;
      else  return ONIGERR_INVALID_CALLOUT_ARG;
      break;
    default:
      return ONIGERR_INVALID_CALLOUT_ARG;
      break;
    }
    val.l = (long )op;
    r = onig_set_callout_data_by_callout_args_self(args, slot, ONIG_TYPE_LONG, &val);
    if (r != ONIG_NORMAL) return r;
  }
  else {
    op = (enum OP_CMP )val.l;
  }

  switch (op) {
  case OP_EQ: r = (lv == rv); break;
  case OP_NE: r = (lv != rv); break;
  case OP_LT: r = (lv <  rv); break;
  case OP_GT: r = (lv >  rv); break;
  case OP_LE: r = (lv <= rv); break;
  case OP_GE: r = (lv >= rv); break;
  }

  return r == 0 ? ONIG_CALLOUT_FAIL : ONIG_CALLOUT_SUCCESS;
}