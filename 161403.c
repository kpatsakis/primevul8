php_stream *phar_wrapper_open_dir(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC) /* {{{ */
{
	php_url *resource = NULL;
	php_stream *ret;
	char *internal_file, *error, *str_key;
	phar_zstr key;
	uint keylen;
	ulong unused;
	phar_archive_data *phar;
	phar_entry_info *entry = NULL;
	uint host_len;

	if ((resource = phar_parse_url(wrapper, path, mode, options TSRMLS_CC)) == NULL) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar url \"%s\" is unknown", path);
		return NULL;
	}

	/* we must have at the very least phar://alias.phar/ */
	if (!resource->scheme || !resource->host || !resource->path) {
		if (resource->host && !resource->path) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: no directory in \"%s\", must have at least phar://%s/ for root directory (always use full path to a new phar)", path, resource->host);
			php_url_free(resource);
			return NULL;
		}
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\", must have at least phar://%s/", path, path);
		return NULL;
	}

	if (strcasecmp("phar", resource->scheme)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar url \"%s\"", path);
		return NULL;
	}

	host_len = strlen(resource->host);
	phar_request_initialize(TSRMLS_C);
	internal_file = resource->path + 1; /* strip leading "/" */

	if (FAILURE == phar_get_archive(&phar, resource->host, host_len, NULL, 0, &error TSRMLS_CC)) {
		if (error) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "%s", error);
			efree(error);
		} else {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar file \"%s\" is unknown", resource->host);
		}
		php_url_free(resource);
		return NULL;
	}

	if (error) {
		efree(error);
	}

	if (*internal_file == '\0') {
		/* root directory requested */
		internal_file = estrndup(internal_file - 1, 1);
		ret = phar_make_dirstream(internal_file, &phar->manifest TSRMLS_CC);
		php_url_free(resource);
		return ret;
	}

	if (!phar->manifest.arBuckets) {
		php_url_free(resource);
		return NULL;
	}

	if (SUCCESS == zend_hash_find(&phar->manifest, internal_file, strlen(internal_file), (void**)&entry) && !entry->is_dir) {
		php_url_free(resource);
		return NULL;
	} else if (entry && entry->is_dir) {
		if (entry->is_mounted) {
			php_url_free(resource);
			return php_stream_opendir(entry->tmp, options, context);
		}
		internal_file = estrdup(internal_file);
		php_url_free(resource);
		return phar_make_dirstream(internal_file, &phar->manifest TSRMLS_CC);
	} else {
		int i_len = strlen(internal_file);

		/* search for directory */
		zend_hash_internal_pointer_reset(&phar->manifest);
		while (FAILURE != zend_hash_has_more_elements(&phar->manifest)) {
			if (HASH_KEY_NON_EXISTENT != 
					zend_hash_get_current_key_ex(
						&phar->manifest, &key, &keylen, &unused, 0, NULL)) {
				PHAR_STR(key, str_key);
				if (keylen > (uint)i_len && 0 == memcmp(str_key, internal_file, i_len)) {
					PHAR_STR_FREE(str_key);
					/* directory found */
					internal_file = estrndup(internal_file,
							i_len);
					php_url_free(resource);
					return phar_make_dirstream(internal_file, &phar->manifest TSRMLS_CC);
				}
				PHAR_STR_FREE(str_key);
			}

			if (SUCCESS != zend_hash_move_forward(&phar->manifest)) {
				break;
			}
		}
	}

	php_url_free(resource);
	return NULL;
}