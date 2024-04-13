static int php_curl_ssl_mutex_destroy(void **m)
{
	tsrm_mutex_free(*((MUTEX_T *) m));
	return SUCCESS;
}