static void php_curl_ssl_lock(int mode, int n, const char * file, int line)
{
	if (mode & CRYPTO_LOCK) {
		tsrm_mutex_lock(php_curl_openssl_tsl[n]);
	} else {
		tsrm_mutex_unlock(php_curl_openssl_tsl[n]);
	}
}