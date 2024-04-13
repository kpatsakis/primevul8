cc_char_next(CClassNode* cc, OnigCodePoint *from, OnigCodePoint to,
             int* from_raw, int to_raw, CVAL intype, CVAL* type,
             CSTATE* state, ScanEnv* env)
{
  int r;

  switch (*state) {
  case CS_VALUE:
    if (*type == CV_SB) {
      if (*from > 0xff)
          return ONIGERR_INVALID_CODE_POINT_VALUE;

      BITSET_SET_BIT(cc->bs, (int )(*from));
    }
    else if (*type == CV_MB) {
      r = add_code_range(&(cc->mbuf), env, *from, *from);
      if (r < 0) return r;
    }
    break;

  case CS_RANGE:
    if (intype == *type) {
      if (intype == CV_SB) {
        if (*from > 0xff || to > 0xff)
          return ONIGERR_INVALID_CODE_POINT_VALUE;

        if (*from > to) {
          if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
            goto ccs_range_end;
          else
            return ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS;
        }
        bitset_set_range(cc->bs, (int )*from, (int )to);
      }
      else {
        r = add_code_range(&(cc->mbuf), env, *from, to);
        if (r < 0) return r;
      }
    }
    else {
      if (*from > to) {
        if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
          goto ccs_range_end;
        else
          return ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS;
      }
      bitset_set_range(cc->bs, (int )*from, (int )(to < 0xff ? to : 0xff));
      r = add_code_range(&(cc->mbuf), env, (OnigCodePoint )*from, to);
      if (r < 0) return r;
    }
  ccs_range_end:
    *state = CS_COMPLETE;
    break;

  case CS_COMPLETE:
  case CS_START:
    *state = CS_VALUE;
    break;

  default:
    break;
  }

  *from_raw = to_raw;
  *from     = to;
  *type     = intype;
  return 0;
}