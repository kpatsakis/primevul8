int phar_wrapper_rmdir(php_stream_wrapper *wrapper, char *url, int options, php_stream_context *context TSRMLS_DC) /* {{{ */
{
	phar_entry_info *entry;
	phar_archive_data *phar = NULL;
	char *error, *arch, *entry2;
	int arch_len, entry_len;
	php_url *resource = NULL;
	uint host_len;
	phar_zstr key;
	char *str_key;
	uint key_len;
	ulong unused;
	uint path_len;

	/* pre-readonly check, we need to know if this is a data phar */
	if (FAILURE == phar_split_fname(url, strlen(url), &arch, &arch_len, &entry2, &entry_len, 2, 2 TSRMLS_CC)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot remove directory \"%s\", no phar archive specified, or phar archive does not exist", url);
		return 0;
	}

	if (FAILURE == phar_get_archive(&phar, arch, arch_len, NULL, 0, NULL TSRMLS_CC)) {
		phar = NULL;
	}

	efree(arch);
	efree(entry2);

	if (PHAR_G(readonly) && (!phar || !phar->is_data)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot rmdir directory \"%s\", write operations disabled", url);
		return 0;
	}

	if ((resource = phar_parse_url(wrapper, url, "w", options TSRMLS_CC)) == NULL) {
		return 0;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource->scheme || !resource->host || !resource->path) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\"", url);
		return 0;
	}

	if (strcasecmp("phar", resource->scheme)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar stream url \"%s\"", url);
		return 0;
	}

	host_len = strlen(resource->host);

	if (FAILURE == phar_get_archive(&phar, resource->host, host_len, NULL, 0, &error TSRMLS_CC)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot remove directory \"%s\" in phar \"%s\", error retrieving phar information: %s", resource->path+1, resource->host, error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	path_len = strlen(resource->path+1);

	if (!(entry = phar_get_entry_info_dir(phar, resource->path + 1, path_len, 2, &error, 1 TSRMLS_CC))) {
		if (error) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot remove directory \"%s\" in phar \"%s\", %s", resource->path+1, resource->host, error);
			efree(error);
		} else {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot remove directory \"%s\" in phar \"%s\", directory does not exist", resource->path+1, resource->host);
		}
		php_url_free(resource);
		return 0;
	}

	if (!entry->is_deleted) {
		for (zend_hash_internal_pointer_reset(&phar->manifest);
		HASH_KEY_NON_EXISTENT != zend_hash_get_current_key_ex(&phar->manifest, &key, &key_len, &unused, 0, NULL);
		zend_hash_move_forward(&phar->manifest)) {

			PHAR_STR(key, str_key);

			if (key_len > path_len && 
				memcmp(str_key, resource->path+1, path_len) == 0 && 
				IS_SLASH(str_key[path_len])) {
				PHAR_STR_FREE(str_key);
				php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: Directory not empty");
				if (entry->is_temp_dir) {
					efree(entry->filename);
					efree(entry);
				}
				php_url_free(resource);
				return 0;
			}
			PHAR_STR_FREE(str_key);
		}

		for (zend_hash_internal_pointer_reset(&phar->virtual_dirs);
			HASH_KEY_NON_EXISTENT != zend_hash_get_current_key_ex(&phar->virtual_dirs, &key, &key_len, &unused, 0, NULL);
			zend_hash_move_forward(&phar->virtual_dirs)) {
	
			PHAR_STR(key, str_key);
	
			if (key_len > path_len && 
				memcmp(str_key, resource->path+1, path_len) == 0 && 
				IS_SLASH(str_key[path_len])) {
				PHAR_STR_FREE(str_key);
				php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: Directory not empty");
				if (entry->is_temp_dir) {
					efree(entry->filename);
					efree(entry);
				}
				php_url_free(resource);
				return 0;
			}
			PHAR_STR_FREE(str_key);
		}
	}

	if (entry->is_temp_dir) {
		zend_hash_del(&phar->virtual_dirs, resource->path+1, path_len);
		efree(entry->filename);
		efree(entry);
	} else {
		entry->is_deleted = 1;
		entry->is_modified = 1;
		phar_flush(phar, 0, 0, 0, &error TSRMLS_CC);

		if (error) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot remove directory \"%s\" in phar \"%s\", %s", entry->filename, phar->fname, error);
			php_url_free(resource);
			efree(error);
			return 0;
		}
	}

	php_url_free(resource);
	return 1;
}