call_plugin_register_handoff(gpointer data, gpointer user_data _U_)
{
	proto_plugin *plug = (proto_plugin *)data;

	if (plug->register_handoff) {
		plug->register_handoff();
	}
}