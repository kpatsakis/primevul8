node_new_general_newline(Node** node, ScanEnv* env)
{
  int r;
  int dlen, alen;
  UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN * 2];
  Node* crnl;
  Node* ncc;
  Node* x;
  CClassNode* cc;

  dlen = ONIGENC_CODE_TO_MBC(env->enc, 0x0d, buf);
  if (dlen < 0) return dlen;
  alen = ONIGENC_CODE_TO_MBC(env->enc, 0x0a, buf + dlen);
  if (alen < 0) return alen;

  crnl = node_new_str_crude(buf, buf + dlen + alen);
  CHECK_NULL_RETURN_MEMERR(crnl);

  ncc = node_new_cclass();
  if (IS_NULL(ncc)) goto err2;

  cc = CCLASS_(ncc);
  if (dlen == 1) {
    bitset_set_range(cc->bs, 0x0a, 0x0d);
  }
  else {
    r = add_code_range(&(cc->mbuf), env, 0x0a, 0x0d);
    if (r != 0) {
    err1:
      onig_node_free(ncc);
    err2:
      onig_node_free(crnl);
      return ONIGERR_MEMORY;
    }
  }

  if (ONIGENC_IS_UNICODE_ENCODING(env->enc)) {
    r = add_code_range(&(cc->mbuf), env, 0x85, 0x85);
    if (r != 0) goto err1;
    r = add_code_range(&(cc->mbuf), env, 0x2028, 0x2029);
    if (r != 0) goto err1;
  }

  x = node_new_bag_if_else(crnl, NULL_NODE, ncc);
  if (IS_NULL(x)) goto err1;

  *node = x;
  return 0;
}