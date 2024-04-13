static int php_curl_option_url(php_curl *ch, const char *url, const int len TSRMLS_DC) /* {{{ */
{
	/* Disable file:// if open_basedir are used */
	if (PG(open_basedir) && *PG(open_basedir)) {
#if LIBCURL_VERSION_NUM >= 0x071304
		curl_easy_setopt(ch->cp, CURLOPT_PROTOCOLS, CURLPROTO_ALL & ~CURLPROTO_FILE);
#else
		php_url *uri;

		if (!(uri = php_url_parse_ex(url, len))) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid URL '%s'", url);
			return FAILURE;
		}

		if (uri->scheme && !strncasecmp("file", uri->scheme, sizeof("file"))) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Protocol 'file' disabled in cURL");
			php_url_free(uri);
			return FAILURE;
		}
		php_url_free(uri);
#endif
	}

	return php_curl_option_str(ch, CURLOPT_URL, url, len, 0 TSRMLS_CC);
}