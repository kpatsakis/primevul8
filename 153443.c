CLOSE_BRACKET_WITHOUT_ESC_WARN(ScanEnv* env, UChar* c)
{
  if (onig_warn == onig_null_warn) return ;

  if (IS_SYNTAX_BV((env)->syntax, ONIG_SYN_WARN_CC_OP_NOT_ESCAPED)) {
    UChar buf[WARN_BUFSIZE];
    onig_snprintf_with_pattern(buf, WARN_BUFSIZE, (env)->enc,
                         (env)->pattern, (env)->pattern_end,
                         (UChar* )"regular expression has '%s' without escape", c);
    (*onig_warn)((char* )buf);
  }
}