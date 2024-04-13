PHP_FUNCTION(curl_exec)
{
	CURLcode	error;
	zval		*zid;
	php_curl	*ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zid) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ch, php_curl *, &zid, -1, le_curl_name, le_curl);

	_php_curl_verify_handlers(ch, 1 TSRMLS_CC);

	_php_curl_cleanup_handle(ch);

	error = curl_easy_perform(ch->cp);
	SAVE_CURL_ERROR(ch, error);
	/* CURLE_PARTIAL_FILE is returned by HEAD requests */
	if (error != CURLE_OK && error != CURLE_PARTIAL_FILE) {
		if (ch->handlers->write->buf.len > 0) {
			smart_str_free(&ch->handlers->write->buf);
		}
		RETURN_FALSE;
	}

	if (ch->handlers->std_err) {
		php_stream  *stream;
		stream = (php_stream*)zend_fetch_resource(&ch->handlers->std_err TSRMLS_CC, -1, NULL, NULL, 2, php_file_le_stream(), php_file_le_pstream());
		if (stream) {
			php_stream_flush(stream);
		}
	}

	if (ch->handlers->write->method == PHP_CURL_RETURN && ch->handlers->write->buf.len > 0) {
		smart_str_0(&ch->handlers->write->buf);
		RETURN_STRINGL(ch->handlers->write->buf.c, ch->handlers->write->buf.len, 1);
	}

	/* flush the file handle, so any remaining data is synched to disk */
	if (ch->handlers->write->method == PHP_CURL_FILE && ch->handlers->write->fp) {
		fflush(ch->handlers->write->fp);
	}
	if (ch->handlers->write_header->method == PHP_CURL_FILE && ch->handlers->write_header->fp) {
		fflush(ch->handlers->write_header->fp);
	}

	if (ch->handlers->write->method == PHP_CURL_RETURN) {
		RETURN_EMPTY_STRING();
	} else {
		RETURN_TRUE;
	}
}