static void __http_protocol_init(void)
{
	acl_register_keywords(&acl_kws);
	sample_register_fetches(&sample_fetch_keywords);
}