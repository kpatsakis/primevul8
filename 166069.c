static int php_curl_option_str(php_curl *ch, long option, const char *str, const int len, zend_bool make_copy TSRMLS_DC)
{
	CURLcode error = CURLE_OK;

	if (strlen(str) != len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Curl option contains invalid characters (\\0)");
		return FAILURE;
	}

#if LIBCURL_VERSION_NUM >= 0x071100
	if (make_copy) {
#endif
		char *copystr;

		/* Strings passed to libcurl as 'char *' arguments, are copied by the library since 7.17.0 */
		copystr = estrndup(str, len);
		error = curl_easy_setopt(ch->cp, option, copystr);
		zend_llist_add_element(&ch->to_free->str, &copystr);
#if LIBCURL_VERSION_NUM >= 0x071100
	} else {
		error = curl_easy_setopt(ch->cp, option, str);
	}
#endif

	SAVE_CURL_ERROR(ch, error)

	return error == CURLE_OK ? SUCCESS : FAILURE;
}