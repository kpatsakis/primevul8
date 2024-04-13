PHP_MSHUTDOWN_FUNCTION(curl)
{
	curl_global_cleanup();
#ifdef PHP_CURL_NEED_OPENSSL_TSL
	if (php_curl_openssl_tsl) {
		int i, c = CRYPTO_num_locks();

		CRYPTO_set_id_callback(NULL);
		CRYPTO_set_locking_callback(NULL);

		for (i = 0; i < c; ++i) {
			tsrm_mutex_free(php_curl_openssl_tsl[i]);
		}

		free(php_curl_openssl_tsl);
		php_curl_openssl_tsl = NULL;
	}
#endif
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}