i_apply_case_fold(OnigCodePoint from, OnigCodePoint to[], int to_len, void* arg)
{
  IApplyCaseFoldArg* iarg;
  ScanEnv* env;
  CClassNode* cc;

  iarg = (IApplyCaseFoldArg* )arg;
  env = iarg->env;
  cc  = iarg->cc;

  if (to_len == 1) {
    int is_in = onig_is_code_in_cc(env->enc, from, cc);
#ifdef CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS
    if ((is_in != 0 && !IS_NCCLASS_NOT(cc)) ||
        (is_in == 0 &&  IS_NCCLASS_NOT(cc))) {
      ADD_CODE_INTO_CC(cc, *to, env->enc);
    }
#else
    if (is_in != 0) {
      if (ONIGENC_MBC_MINLEN(env->enc) > 1 ||
          ONIGENC_CODE_TO_MBCLEN(env->enc, *to) != 1) {
        if (IS_NCCLASS_NOT(cc)) clear_not_flag_cclass(cc, env->enc);
        add_code_range(&(cc->mbuf), env, *to, *to);
      }
      else {
        if (IS_NCCLASS_NOT(cc)) {
          BITSET_CLEAR_BIT(cc->bs, *to);
        }
        else
          BITSET_SET_BIT(cc->bs, *to);
      }
    }
#endif /* CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS */
  }
  else {
    int r, i, len;
    UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];

    if (onig_is_code_in_cc(env->enc, from, cc)
#ifdef CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS
        && !IS_NCCLASS_NOT(cc)
#endif
        ) {
      int n, j, m, index;
      Node* list_node;
      Node* ns[3];

      n = 0;
      for (i = 0; i < to_len; i++) {
        OnigCodePoint code;
        Node* csnode;
        CClassNode* cs_cc;

        index = onigenc_unicode_fold1_key(&to[i]);
        if (index >= 0) {
          csnode = node_new_cclass();
          cs_cc = CCLASS_(csnode);
          if (IS_NULL(csnode)) {
          err_free_ns:
            for (j = 0; j < n; j++) onig_node_free(ns[j]);
            return ONIGERR_MEMORY;
          }
          m = FOLDS1_UNFOLDS_NUM(index);
          for (j = 0; j < m; j++) {
            code = FOLDS1_UNFOLDS(index)[j];
            ADD_CODE_INTO_CC(cs_cc, code, env->enc);
          }
          ADD_CODE_INTO_CC(cs_cc, to[i], env->enc);
          ns[n++] = csnode;
        }
        else {
          len = ONIGENC_CODE_TO_MBC(env->enc, to[i], buf);
          if (n == 0 || NODE_TYPE(ns[n-1]) != NODE_STRING) {
            csnode = onig_node_new_str(buf, buf + len);
            if (IS_NULL(csnode)) goto err_free_ns;

            NODE_STRING_SET_CASE_EXPANDED(csnode);
            ns[n++] = csnode;
          }
          else {
            r = onig_node_str_cat(ns[n-1], buf, buf + len);
            if (r < 0) goto err_free_ns;
          }
        }
      }

      if (n == 1)
        list_node = ns[0];
      else
        list_node = make_list(n, ns);

      *(iarg->ptail) = onig_node_new_alt(list_node, NULL_NODE);
      if (IS_NULL(*(iarg->ptail))) {
        onig_node_free(list_node);
        return ONIGERR_MEMORY;
      }
      iarg->ptail = &(NODE_CDR((*(iarg->ptail))));
    }
  }

  return 0;
}