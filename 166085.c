static size_t curl_write_nothing(char *data, size_t size, size_t nmemb, void *ctx)
{
	return size * nmemb;
}