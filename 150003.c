onig_syntax_warn(ScanEnv *env, const char *fmt, ...)
{
    va_list args;
    UChar buf[WARN_BUFSIZE];
    va_start(args, fmt);
    onig_vsnprintf_with_pattern(buf, WARN_BUFSIZE, env->enc,
		env->pattern, env->pattern_end,
		(const UChar *)fmt, args);
    va_end(args);
#ifdef RUBY
    if (env->sourcefile == NULL)
      rb_warn("%s", (char *)buf);
    else
      rb_compile_warn(env->sourcefile, env->sourceline, "%s", (char *)buf);
#else
    (*onig_warn)((char* )buf);
#endif
}