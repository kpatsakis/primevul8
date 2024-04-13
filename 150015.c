onig_parse_make_tree(Node** root, const UChar* pattern, const UChar* end,
		     regex_t* reg, ScanEnv* env)
{
  int r;
  UChar* p;

#ifdef USE_NAMED_GROUP
  names_clear(reg);
#endif

  scan_env_clear(env);
  env->option         = reg->options;
  env->case_fold_flag = reg->case_fold_flag;
  env->enc            = reg->enc;
  env->syntax         = reg->syntax;
  env->pattern        = (UChar* )pattern;
  env->pattern_end    = (UChar* )end;
  env->reg            = reg;

  *root = NULL;
  p = (UChar* )pattern;
  r = parse_regexp(root, &p, (UChar* )end, env);
  reg->num_mem = env->num_mem;
  return r;
}