static void
php_mysqlnd_ok_free_mem(void * _packet)
{
	MYSQLND_PACKET_OK *p= (MYSQLND_PACKET_OK *) _packet;
	if (p->message) {
		mnd_efree(p->message);
		p->message = NULL;
	}