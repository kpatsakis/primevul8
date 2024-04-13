cc_cprop_next(CClassNode* cc, OnigCodePoint* pcode, CVAL* val, CSTATE* state,
              ScanEnv* env)
{
  int r;

  if (*state == CS_RANGE)
    return ONIGERR_CHAR_CLASS_VALUE_AT_END_OF_RANGE;

  if (*state == CS_VALUE) {
    if (*val == CV_SB)
      BITSET_SET_BIT(cc->bs, (int )(*pcode));
    else if (*val == CV_MB) {
      r = add_code_range(&(cc->mbuf), env, *pcode, *pcode);
      if (r < 0) return r;
    }
  }

  *state = CS_VALUE;
  *val   = CV_CPROP;
  return 0;
}