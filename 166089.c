static int php_curl_ssl_mutex_unlock(void **m)
{
	return tsrm_mutex_unlock(*((MUTEX_T *) m));
}