check_fmt(struct magic_set *ms, struct magic *m)
{
	pcre *pce;
	int re_options;
	pcre_extra *re_extra;
	TSRMLS_FETCH();
	
	if (strchr(m->desc, '%') == NULL) {
		return 0;
	}
	
	if ((pce = pcre_get_compiled_regex("~%[-0-9.]*s~", &re_extra, &re_options TSRMLS_CC)) == NULL) {
		return -1;
	} else {
	 	return !pcre_exec(pce, re_extra, m->desc, strlen(m->desc), 0, re_options, NULL, 0);
	}
}