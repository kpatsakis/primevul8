static
void php_mysqlnd_stats_free_mem(void * _packet)
{
	MYSQLND_PACKET_STATS *p= (MYSQLND_PACKET_STATS *) _packet;
	if (p->message.s) {
		mnd_efree(p->message.s);
		p->message.s = NULL;
	}