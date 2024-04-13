PHP_FUNCTION(curl_copy_handle)
{
	CURL		*cp;
	zval		*zid;
	php_curl	*ch, *dupch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zid) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ch, php_curl *, &zid, -1, le_curl_name, le_curl);

	cp = curl_easy_duphandle(ch->cp);
	if (!cp) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot duplicate cURL handle");
		RETURN_FALSE;
	}

	alloc_curl_handle(&dupch);
	TSRMLS_SET_CTX(dupch->thread_ctx);

	dupch->cp = cp;
	zend_list_addref(Z_LVAL_P(zid));
	if (ch->handlers->write->stream) {
		Z_ADDREF_P(ch->handlers->write->stream);
	}
	dupch->handlers->write->stream = ch->handlers->write->stream;
	dupch->handlers->write->method = ch->handlers->write->method;
	if (ch->handlers->read->stream) {
		Z_ADDREF_P(ch->handlers->read->stream);
	}
	dupch->handlers->read->stream  = ch->handlers->read->stream;
	dupch->handlers->read->method  = ch->handlers->read->method;
	dupch->handlers->write_header->method = ch->handlers->write_header->method;
	if (ch->handlers->write_header->stream) {
		Z_ADDREF_P(ch->handlers->write_header->stream);
	}
	dupch->handlers->write_header->stream = ch->handlers->write_header->stream;

	dupch->handlers->write->fp = ch->handlers->write->fp;
	dupch->handlers->write_header->fp = ch->handlers->write_header->fp;
	dupch->handlers->read->fp = ch->handlers->read->fp;
	dupch->handlers->read->fd = ch->handlers->read->fd;
#if CURLOPT_PASSWDDATA != 0
	if (ch->handlers->passwd) {
		zval_add_ref(&ch->handlers->passwd);
		dupch->handlers->passwd = ch->handlers->passwd;
		curl_easy_setopt(ch->cp, CURLOPT_PASSWDDATA, (void *) dupch);
	}
#endif
	if (ch->handlers->write->func_name) {
		zval_add_ref(&ch->handlers->write->func_name);
		dupch->handlers->write->func_name = ch->handlers->write->func_name;
	}
	if (ch->handlers->read->func_name) {
		zval_add_ref(&ch->handlers->read->func_name);
		dupch->handlers->read->func_name = ch->handlers->read->func_name;
	}
	if (ch->handlers->write_header->func_name) {
		zval_add_ref(&ch->handlers->write_header->func_name);
		dupch->handlers->write_header->func_name = ch->handlers->write_header->func_name;
	}

	curl_easy_setopt(dupch->cp, CURLOPT_ERRORBUFFER,       dupch->err.str);
	curl_easy_setopt(dupch->cp, CURLOPT_FILE,              (void *) dupch);
	curl_easy_setopt(dupch->cp, CURLOPT_INFILE,            (void *) dupch);
	curl_easy_setopt(dupch->cp, CURLOPT_WRITEHEADER,       (void *) dupch);

	if (ch->handlers->progress) {
		dupch->handlers->progress = ecalloc(1, sizeof(php_curl_progress));
		if (ch->handlers->progress->func_name) {
			zval_add_ref(&ch->handlers->progress->func_name);
			dupch->handlers->progress->func_name = ch->handlers->progress->func_name;
		}
		dupch->handlers->progress->method = ch->handlers->progress->method;
		curl_easy_setopt(dupch->cp, CURLOPT_PROGRESSDATA, (void *) dupch);
	}

/* Available since 7.21.0 */
#if LIBCURL_VERSION_NUM >= 0x071500
	if (ch->handlers->fnmatch) {
		dupch->handlers->fnmatch = ecalloc(1, sizeof(php_curl_fnmatch));
		if (ch->handlers->fnmatch->func_name) {
			zval_add_ref(&ch->handlers->fnmatch->func_name);
			dupch->handlers->fnmatch->func_name = ch->handlers->fnmatch->func_name;
		}
		dupch->handlers->fnmatch->method = ch->handlers->fnmatch->method;
		curl_easy_setopt(dupch->cp, CURLOPT_FNMATCH_DATA, (void *) dupch);
	}
#endif

	efree(dupch->to_free->slist);
	efree(dupch->to_free);
	dupch->to_free = ch->to_free;

	/* Keep track of cloned copies to avoid invoking curl destructors for every clone */
	Z_ADDREF_P(ch->clone);
	dupch->clone = ch->clone;

	ZEND_REGISTER_RESOURCE(return_value, dupch, le_curl);
	dupch->id = Z_LVAL_P(return_value);
}