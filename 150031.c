i_apply_case_fold(OnigCodePoint from, OnigCodePoint to[],
		  int to_len, void* arg)
{
  IApplyCaseFoldArg* iarg;
  ScanEnv* env;
  CClassNode* cc;
  CClassNode* asc_cc;
  BitSetRef bs;
  int add_flag, r;

  iarg = (IApplyCaseFoldArg* )arg;
  env = iarg->env;
  cc  = iarg->cc;
  asc_cc = iarg->asc_cc;
  bs = cc->bs;

  if (IS_NULL(asc_cc)) {
    add_flag = 0;
  }
  else if (ONIGENC_IS_ASCII_CODE(from) == ONIGENC_IS_ASCII_CODE(*to)) {
    add_flag = 1;
  }
  else {
    add_flag = onig_is_code_in_cc(env->enc, from, asc_cc);
    if (IS_NCCLASS_NOT(asc_cc))
      add_flag = !add_flag;
  }

  if (to_len == 1) {
    int is_in = onig_is_code_in_cc(env->enc, from, cc);
#ifdef CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS
    if ((is_in != 0 && !IS_NCCLASS_NOT(cc)) ||
	(is_in == 0 &&  IS_NCCLASS_NOT(cc))) {
      if (add_flag) {
	if (ONIGENC_MBC_MINLEN(env->enc) > 1 || *to >= SINGLE_BYTE_SIZE) {
	  r = add_code_range0(&(cc->mbuf), env, *to, *to, 0);
	  if (r < 0) return r;
	}
	else {
	  BITSET_SET_BIT(bs, *to);
	}
      }
    }
#else
    if (is_in != 0) {
      if (add_flag) {
	if (ONIGENC_MBC_MINLEN(env->enc) > 1 || *to >= SINGLE_BYTE_SIZE) {
	  if (IS_NCCLASS_NOT(cc)) clear_not_flag_cclass(cc, env->enc);
	  r = add_code_range0(&(cc->mbuf), env, *to, *to, 0);
	  if (r < 0) return r;
	}
	else {
	  if (IS_NCCLASS_NOT(cc)) {
	    BITSET_CLEAR_BIT(bs, *to);
	  }
	  else {
	    BITSET_SET_BIT(bs, *to);
	  }
	}
      }
    }
#endif /* CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS */
  }
  else {
    int r, i, len;
    UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];
    Node *snode = NULL_NODE;

    if (onig_is_code_in_cc(env->enc, from, cc)
#ifdef CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS
	&& !IS_NCCLASS_NOT(cc)
#endif
	) {
      for (i = 0; i < to_len; i++) {
	len = ONIGENC_CODE_TO_MBC(env->enc, to[i], buf);
	if (i == 0) {
	  snode = onig_node_new_str(buf, buf + len);
	  CHECK_NULL_RETURN_MEMERR(snode);

	  /* char-class expanded multi-char only
	     compare with string folded at match time. */
	  NSTRING_SET_AMBIG(snode);
	}
	else {
	  r = onig_node_str_cat(snode, buf, buf + len);
	  if (r < 0) {
	    onig_node_free(snode);
	    return r;
	  }
	}
      }

      *(iarg->ptail) = onig_node_new_alt(snode, NULL_NODE);
      CHECK_NULL_RETURN_MEMERR(*(iarg->ptail));
      iarg->ptail = &(NCDR((*(iarg->ptail))));
    }
  }

  return 0;
}