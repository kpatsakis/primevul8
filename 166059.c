PHP_FUNCTION(curl_getinfo)
{
	zval		*zid;
	php_curl	*ch;
	long		option = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &zid, &option) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ch, php_curl *, &zid, -1, le_curl_name, le_curl);

	if (ZEND_NUM_ARGS() < 2) {
		char   *s_code;
		long    l_code;
		double  d_code;
#if LIBCURL_VERSION_NUM >  0x071301
		struct curl_certinfo *ci = NULL;
		zval *listcode;
#endif

		array_init(return_value);

		if (curl_easy_getinfo(ch->cp, CURLINFO_EFFECTIVE_URL, &s_code) == CURLE_OK) {
			CAAS("url", s_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_CONTENT_TYPE, &s_code) == CURLE_OK) {
			if (s_code != NULL) {
				CAAS("content_type", s_code);
			} else {
				zval *retnull;
				MAKE_STD_ZVAL(retnull);
				ZVAL_NULL(retnull);
				CAAZ("content_type", retnull);
			}
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_HTTP_CODE, &l_code) == CURLE_OK) {
			CAAL("http_code", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_HEADER_SIZE, &l_code) == CURLE_OK) {
			CAAL("header_size", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_REQUEST_SIZE, &l_code) == CURLE_OK) {
			CAAL("request_size", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_FILETIME, &l_code) == CURLE_OK) {
			CAAL("filetime", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_SSL_VERIFYRESULT, &l_code) == CURLE_OK) {
			CAAL("ssl_verify_result", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_REDIRECT_COUNT, &l_code) == CURLE_OK) {
			CAAL("redirect_count", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_TOTAL_TIME, &d_code) == CURLE_OK) {
			CAAD("total_time", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_NAMELOOKUP_TIME, &d_code) == CURLE_OK) {
			CAAD("namelookup_time", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_CONNECT_TIME, &d_code) == CURLE_OK) {
			CAAD("connect_time", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_PRETRANSFER_TIME, &d_code) == CURLE_OK) {
			CAAD("pretransfer_time", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_SIZE_UPLOAD, &d_code) == CURLE_OK) {
			CAAD("size_upload", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_SIZE_DOWNLOAD, &d_code) == CURLE_OK) {
			CAAD("size_download", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_SPEED_DOWNLOAD, &d_code) == CURLE_OK) {
			CAAD("speed_download", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_SPEED_UPLOAD, &d_code) == CURLE_OK) {
			CAAD("speed_upload", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &d_code) == CURLE_OK) {
			CAAD("download_content_length", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_CONTENT_LENGTH_UPLOAD, &d_code) == CURLE_OK) {
			CAAD("upload_content_length", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_STARTTRANSFER_TIME, &d_code) == CURLE_OK) {
			CAAD("starttransfer_time", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_REDIRECT_TIME, &d_code) == CURLE_OK) {
			CAAD("redirect_time", d_code);
		}
#if LIBCURL_VERSION_NUM >= 0x071202 /* Available since 7.18.2 */
		if (curl_easy_getinfo(ch->cp, CURLINFO_REDIRECT_URL, &s_code) == CURLE_OK) {
			CAAS("redirect_url", s_code);
		}
#endif
#if LIBCURL_VERSION_NUM >= 0x071300 /* Available since 7.19.0 */
		if (curl_easy_getinfo(ch->cp, CURLINFO_PRIMARY_IP, &s_code) == CURLE_OK) {
			CAAS("primary_ip", s_code);
		}
#endif
#if LIBCURL_VERSION_NUM >= 0x071301 /* Available since 7.19.1 */
		if (curl_easy_getinfo(ch->cp, CURLINFO_CERTINFO, &ci) == CURLE_OK) {
			MAKE_STD_ZVAL(listcode);
			array_init(listcode);
			create_certinfo(ci, listcode TSRMLS_CC);
			CAAZ("certinfo", listcode);
		}
#endif
#if LIBCURL_VERSION_NUM >= 0x071500 /* Available since 7.21.0 */
		if (curl_easy_getinfo(ch->cp, CURLINFO_PRIMARY_PORT, &l_code) == CURLE_OK) {
			CAAL("primary_port", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_LOCAL_IP, &s_code) == CURLE_OK) {
			CAAS("local_ip", s_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_LOCAL_PORT, &l_code) == CURLE_OK) {
			CAAL("local_port", l_code);
		}
#endif
		if (ch->header.str_len > 0) {
			CAAS("request_header", ch->header.str);
		}
	} else {
		switch (option) {
			case CURLINFO_HEADER_OUT:
				if (ch->header.str_len > 0) {
					RETURN_STRINGL(ch->header.str, ch->header.str_len, 1);
				} else {
					RETURN_FALSE;
				}
#if LIBCURL_VERSION_NUM >= 0x071301 /* Available since 7.19.1 */
			case CURLINFO_CERTINFO: {
				struct curl_certinfo *ci = NULL;

				array_init(return_value);

				if (curl_easy_getinfo(ch->cp, CURLINFO_CERTINFO, &ci) == CURLE_OK) {
					create_certinfo(ci, return_value TSRMLS_CC);
				} else {
					RETURN_FALSE;
				}
				break;
			}
#endif
			default: {
				int type = CURLINFO_TYPEMASK & option;
				switch (type) {
					case CURLINFO_STRING:
					{
						char *s_code = NULL;

						if (curl_easy_getinfo(ch->cp, option, &s_code) == CURLE_OK && s_code) {
							RETURN_STRING(s_code, 1);
						} else {
							RETURN_FALSE;
						}
						break;
					}
					case CURLINFO_LONG:
					{
						long code = 0;

						if (curl_easy_getinfo(ch->cp, option, &code) == CURLE_OK) {
							RETURN_LONG(code);
						} else {
							RETURN_FALSE;
						}
						break;
					}
					case CURLINFO_DOUBLE:
					{
						double code = 0.0;

						if (curl_easy_getinfo(ch->cp, option, &code) == CURLE_OK) {
							RETURN_DOUBLE(code);
						} else {
							RETURN_FALSE;
						}
						break;
					}
#if LIBCURL_VERSION_NUM >= 0x070c03 /* Available since 7.12.3 */
					case CURLINFO_SLIST:
					{
						struct curl_slist *slist;
						array_init(return_value);
						if (curl_easy_getinfo(ch->cp, option, &slist) == CURLE_OK) {
							while (slist) {
								add_next_index_string(return_value, slist->data, 1);
								slist = slist->next;
							}
							curl_slist_free_all(slist);
						} else {
							RETURN_FALSE;
						}
						break;
					}
#endif
					default:
						RETURN_FALSE;
				}
			}
		}
	}
}