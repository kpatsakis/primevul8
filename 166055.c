static void curl_free_post(void **post)
{
	curl_formfree((struct HttpPost *) *post);
}