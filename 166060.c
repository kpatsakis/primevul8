static void _php_curl_reset_handlers(php_curl *ch)
{
	if (ch->handlers->write->stream) {
		Z_DELREF_P(ch->handlers->write->stream);
		ch->handlers->write->stream = NULL;
	}
	ch->handlers->write->fp = NULL;
	ch->handlers->write->method = PHP_CURL_STDOUT;

	if (ch->handlers->write_header->stream) {
		Z_DELREF_P(ch->handlers->write_header->stream);
		ch->handlers->write_header->stream = NULL;
	}
	ch->handlers->write_header->fp = NULL;
	ch->handlers->write_header->method = PHP_CURL_IGNORE;

	if (ch->handlers->read->stream) {
		Z_DELREF_P(ch->handlers->read->stream);
		ch->handlers->read->stream = NULL;
	}
	ch->handlers->read->fp = NULL;
	ch->handlers->read->fd = 0;
	ch->handlers->read->method  = PHP_CURL_DIRECT;

	if (ch->handlers->std_err) {
		zval_ptr_dtor(&ch->handlers->std_err);
		ch->handlers->std_err = NULL;
	}

	if (ch->handlers->progress) {
		if (ch->handlers->progress->func_name) {
			zval_ptr_dtor(&ch->handlers->progress->func_name);
		}
		efree(ch->handlers->progress);
		ch->handlers->progress = NULL;
	}

#if LIBCURL_VERSION_NUM >= 0x071500 /* Available since 7.21.0 */
	if (ch->handlers->fnmatch) {
		if (ch->handlers->fnmatch->func_name) {
			zval_ptr_dtor(&ch->handlers->fnmatch->func_name);
		}
		efree(ch->handlers->fnmatch);
		ch->handlers->fnmatch = NULL;
	}
#endif

}