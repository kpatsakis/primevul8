static void curl_free_string(void **string)
{
	efree(*string);
}