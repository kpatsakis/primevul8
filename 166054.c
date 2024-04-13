static void create_certinfo(struct curl_certinfo *ci, zval *listcode TSRMLS_DC)
{
	int i;

	if(ci) {
		zval *certhash = NULL;

		for(i=0; i<ci->num_of_certs; i++) {
			struct curl_slist *slist;

			MAKE_STD_ZVAL(certhash);
			array_init(certhash);
			for(slist = ci->certinfo[i]; slist; slist = slist->next) {
				int len;
				char s[64];
				char *tmp;
				strncpy(s, slist->data, 64);
				tmp = memchr(s, ':', 64);
				if(tmp) {
					*tmp = '\0';
					len = strlen(s);
					if(!strcmp(s, "Subject") || !strcmp(s, "Issuer")) {
						zval *hash;

						MAKE_STD_ZVAL(hash);
						array_init(hash);

						split_certinfo(&slist->data[len+1], hash);
						add_assoc_zval(certhash, s, hash);
					} else {
						add_assoc_string(certhash, s, &slist->data[len+1], 1);
					}
				} else {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not extract hash key from certificate info");
				}
			}
			add_next_index_zval(listcode, certhash);
		}
	}
}