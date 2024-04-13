parse_regexp(Node** top, UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  OnigToken tok;

  r = fetch_token(&tok, src, end, env);
  if (r < 0) return r;
  r = parse_subexp(top, &tok, TK_EOT, src, end, env);
  if (r < 0) return r;

#ifdef USE_SUBEXP_CALL
  if (env->num_call > 0) {
    /* Capture the pattern itself. It is used for (?R), (?0) and \g<0>. */
    const int num = 0;
    Node* np;
    np = node_new_enclose_memory(env->option, 0);
    CHECK_NULL_RETURN_MEMERR(np);
    NENCLOSE(np)->regnum = num;
    NENCLOSE(np)->target = *top;
    r = scan_env_set_mem_node(env, num, np);
    if (r != 0) {
	onig_node_free(np);
	return r;
    }
    *top = np;
  }
#endif
  return 0;
}