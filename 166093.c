void _php_curl_verify_handlers(php_curl *ch, int reporterror TSRMLS_DC) /* {{{ */
{
	php_stream *stream;
	if (!ch || !ch->handlers) {
		return;
	}

	if (ch->handlers->std_err) {
		stream = (php_stream *) zend_fetch_resource(&ch->handlers->std_err TSRMLS_CC, -1, NULL, NULL, 2, php_file_le_stream(), php_file_le_pstream());
		if (stream == NULL) {
			if (reporterror) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "CURLOPT_STDERR resource has gone away, resetting to stderr");
			}
			zval_ptr_dtor(&ch->handlers->std_err);
			ch->handlers->std_err = NULL;

			curl_easy_setopt(ch->cp, CURLOPT_STDERR, stderr);
		}
	}
	if (ch->handlers->read && ch->handlers->read->stream) {
		stream = (php_stream *) zend_fetch_resource(&ch->handlers->read->stream TSRMLS_CC, -1, NULL, NULL, 2, php_file_le_stream(), php_file_le_pstream());
		if (stream == NULL) {
			if (reporterror) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "CURLOPT_INFILE resource has gone away, resetting to default");
			}
			zval_ptr_dtor(&ch->handlers->read->stream);
			ch->handlers->read->fd = 0;
			ch->handlers->read->fp = 0;
			ch->handlers->read->stream = NULL;

			curl_easy_setopt(ch->cp, CURLOPT_INFILE, (void *) ch);
		}
	}
	if (ch->handlers->write_header && ch->handlers->write_header->stream) {
		stream = (php_stream *) zend_fetch_resource(&ch->handlers->write_header->stream TSRMLS_CC, -1, NULL, NULL, 2, php_file_le_stream(), php_file_le_pstream());
		if (stream == NULL) {
			if (reporterror) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "CURLOPT_WRITEHEADER resource has gone away, resetting to default");
			}
			zval_ptr_dtor(&ch->handlers->write_header->stream);
			ch->handlers->write_header->fp = 0;
			ch->handlers->write_header->stream = NULL;

			ch->handlers->write_header->method = PHP_CURL_IGNORE;
			curl_easy_setopt(ch->cp, CURLOPT_WRITEHEADER, (void *) ch);
		}
	}
	if (ch->handlers->write && ch->handlers->write->stream) {
		stream = (php_stream *) zend_fetch_resource(&ch->handlers->write->stream TSRMLS_CC, -1, NULL, NULL, 2, php_file_le_stream(), php_file_le_pstream());
		if (stream == NULL) {
			if (reporterror) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "CURLOPT_FILE resource has gone away, resetting to default");
			}
			zval_ptr_dtor(&ch->handlers->write->stream);
			ch->handlers->write->fp = 0;
			ch->handlers->write->stream = NULL;

			ch->handlers->write->method = PHP_CURL_STDOUT;
			curl_easy_setopt(ch->cp, CURLOPT_FILE, (void *) ch);
		}
	}
	return ;
}