call_plugin_register_protoinfo(gpointer data, gpointer user_data _U_)
{
	proto_plugin *plug = (proto_plugin *)data;

	if (plug->register_protoinfo) {
		plug->register_protoinfo();
	}
}