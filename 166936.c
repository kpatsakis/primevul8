proto_cleanup(void)
{
	proto_free_deregistered_fields();
	proto_cleanup_base();

#ifdef HAVE_PLUGINS
	g_slist_free(dissector_plugins);
	dissector_plugins = NULL;
#endif
}