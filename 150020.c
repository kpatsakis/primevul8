next_state_class(CClassNode* cc, CClassNode* asc_cc,
		 OnigCodePoint* vs, enum CCVALTYPE* type,
		 enum CCSTATE* state, ScanEnv* env)
{
  int r;

  if (*state == CCS_RANGE)
    return ONIGERR_CHAR_CLASS_VALUE_AT_END_OF_RANGE;

  if (*state == CCS_VALUE && *type != CCV_CLASS) {
    if (*type == CCV_SB) {
      BITSET_SET_BIT_CHKDUP(cc->bs, (int )(*vs));
      if (IS_NOT_NULL(asc_cc))
	BITSET_SET_BIT(asc_cc->bs, (int )(*vs));
    }
    else if (*type == CCV_CODE_POINT) {
      r = add_code_range(&(cc->mbuf), env, *vs, *vs);
      if (r < 0) return r;
      if (IS_NOT_NULL(asc_cc)) {
	r = add_code_range0(&(asc_cc->mbuf), env, *vs, *vs, 0);
	if (r < 0) return r;
      }
    }
  }

  *state = CCS_VALUE;
  *type  = CCV_CLASS;
  return 0;
}