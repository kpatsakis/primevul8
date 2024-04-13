onig_new_with_source(regex_t** reg, const UChar* pattern, const UChar* pattern_end,
		     OnigOptionType option, OnigEncoding enc, const OnigSyntaxType* syntax,
		     OnigErrorInfo* einfo, const char *sourcefile, int sourceline)
{
    int r;

    *reg = (regex_t* )malloc(sizeof(regex_t));
    if (IS_NULL(*reg)) return ONIGERR_MEMORY;

    r = onig_reg_init(*reg, option, ONIGENC_CASE_FOLD_DEFAULT, enc, syntax);
    if (r) goto err;

    r = onig_compile_ruby(*reg, pattern, pattern_end, einfo, sourcefile, sourceline);
    if (r) {
      err:
	onig_free(*reg);
	*reg = NULL;
    }
    return r;
}