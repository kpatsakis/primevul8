static void alloc_curl_handle(php_curl **ch)
{
	*ch                           = emalloc(sizeof(php_curl));
	(*ch)->to_free                = ecalloc(1, sizeof(struct _php_curl_free));
	(*ch)->handlers               = ecalloc(1, sizeof(php_curl_handlers));
	(*ch)->handlers->write        = ecalloc(1, sizeof(php_curl_write));
	(*ch)->handlers->write_header = ecalloc(1, sizeof(php_curl_write));
	(*ch)->handlers->read         = ecalloc(1, sizeof(php_curl_read));
	(*ch)->handlers->progress     = NULL;
#if LIBCURL_VERSION_NUM >= 0x071500 /* Available since 7.21.0 */
	(*ch)->handlers->fnmatch      = NULL;
#endif

	(*ch)->in_callback = 0;
	(*ch)->header.str_len = 0;

	memset(&(*ch)->err, 0, sizeof((*ch)->err));
	(*ch)->handlers->write->stream = NULL;
	(*ch)->handlers->write_header->stream = NULL;
	(*ch)->handlers->read->stream = NULL;

	zend_llist_init(&(*ch)->to_free->str,   sizeof(char *),            (llist_dtor_func_t) curl_free_string, 0);
	zend_llist_init(&(*ch)->to_free->post,  sizeof(struct HttpPost),   (llist_dtor_func_t) curl_free_post,   0);
	(*ch)->safe_upload = 1; /* for now, for BC reason we allow unsafe API */

	(*ch)->to_free->slist = emalloc(sizeof(HashTable));
	zend_hash_init((*ch)->to_free->slist, 4, NULL, curl_free_slist, 0);
}