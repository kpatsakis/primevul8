int phar_wrapper_mkdir(php_stream_wrapper *wrapper, char *url_from, int mode, int options, php_stream_context *context TSRMLS_DC) /* {{{ */
{
	phar_entry_info entry, *e;
	phar_archive_data *phar = NULL;
	char *error, *arch, *entry2;
	int arch_len, entry_len;
	php_url *resource = NULL;
	uint host_len;

	/* pre-readonly check, we need to know if this is a data phar */
	if (FAILURE == phar_split_fname(url_from, strlen(url_from), &arch, &arch_len, &entry2, &entry_len, 2, 2 TSRMLS_CC)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\", no phar archive specified", url_from);
		return 0;
	}

	if (FAILURE == phar_get_archive(&phar, arch, arch_len, NULL, 0, NULL TSRMLS_CC)) {
		phar = NULL;
	}

	efree(arch);
	efree(entry2);

	if (PHAR_G(readonly) && (!phar || !phar->is_data)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\", write operations disabled", url_from);
		return 0;
	}

	if ((resource = phar_parse_url(wrapper, url_from, "w", options TSRMLS_CC)) == NULL) {
		return 0;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource->scheme || !resource->host || !resource->path) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\"", url_from);
		return 0;
	}

	if (strcasecmp("phar", resource->scheme)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar stream url \"%s\"", url_from);
		return 0;
	}

	host_len = strlen(resource->host);

	if (FAILURE == phar_get_archive(&phar, resource->host, host_len, NULL, 0, &error TSRMLS_CC)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\" in phar \"%s\", error retrieving phar information: %s", resource->path+1, resource->host, error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	if ((e = phar_get_entry_info_dir(phar, resource->path + 1, strlen(resource->path + 1), 2, &error, 1 TSRMLS_CC))) {
		/* directory exists, or is a subdirectory of an existing file */
		if (e->is_temp_dir) {
			efree(e->filename);
			efree(e);
		}
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\" in phar \"%s\", directory already exists", resource->path+1, resource->host);
		php_url_free(resource);
		return 0;
	}

	if (error) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\" in phar \"%s\", %s", resource->path+1, resource->host, error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	if (phar_get_entry_info_dir(phar, resource->path + 1, strlen(resource->path + 1), 0, &error, 1 TSRMLS_CC)) {
		/* entry exists as a file */
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\" in phar \"%s\", file already exists", resource->path+1, resource->host);
		php_url_free(resource);
		return 0;
	}

	if (error) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\" in phar \"%s\", %s", resource->path+1, resource->host, error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	memset((void *) &entry, 0, sizeof(phar_entry_info));

	/* strip leading "/" */
	if (phar->is_zip) {
		entry.is_zip = 1;
	}

	entry.filename = estrdup(resource->path + 1);

	if (phar->is_tar) {
		entry.is_tar = 1;
		entry.tar_type = TAR_DIR;
	}

	entry.filename_len = strlen(resource->path + 1);
	php_url_free(resource);
	entry.is_dir = 1;
	entry.phar = phar;
	entry.is_modified = 1;
	entry.is_crc_checked = 1;
	entry.flags = PHAR_ENT_PERM_DEF_DIR;
	entry.old_flags = PHAR_ENT_PERM_DEF_DIR;

	if (SUCCESS != zend_hash_add(&phar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\" in phar \"%s\", adding to manifest failed", entry.filename, phar->fname);
		efree(error);
		efree(entry.filename);
		return 0;
	}

	phar_flush(phar, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\" in phar \"%s\", %s", entry.filename, phar->fname, error);
		zend_hash_del(&phar->manifest, entry.filename, entry.filename_len);
		efree(error);
		return 0;
	}

	phar_add_virtual_dirs(phar, entry.filename, entry.filename_len TSRMLS_CC);
	return 1;
}