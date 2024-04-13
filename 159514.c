static
void php_mysqlnd_greet_free_mem(void * _packet)
{
	MYSQLND_PACKET_GREET *p= (MYSQLND_PACKET_GREET *) _packet;
	if (p->server_version) {
		efree(p->server_version);
		p->server_version = NULL;
	}
	if (p->authentication_plugin_data.s && p->authentication_plugin_data.s != p->intern_auth_plugin_data) {
		efree(p->authentication_plugin_data.s);
		p->authentication_plugin_data.s = NULL;
	}
	if (p->auth_protocol) {
		efree(p->auth_protocol);
		p->auth_protocol = NULL;
	}