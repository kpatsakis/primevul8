static void curl_free_slist(void *slist)
{
	curl_slist_free_all(*((struct curl_slist **) slist));
}