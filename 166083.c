static unsigned long php_curl_ssl_id(void)
{
	return (unsigned long) tsrm_thread_id();
}