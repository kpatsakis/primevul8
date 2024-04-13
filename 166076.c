static int php_curl_ssl_mutex_lock(void **m)
{
	return tsrm_mutex_lock(*((MUTEX_T *) m));
}