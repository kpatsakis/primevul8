static
void php_mysqlnd_rset_header_free_mem(void * _packet)
{
	MYSQLND_PACKET_RSET_HEADER *p= (MYSQLND_PACKET_RSET_HEADER *) _packet;
	DBG_ENTER("php_mysqlnd_rset_header_free_mem");
	if (p->info_or_local_file.s) {
		mnd_efree(p->info_or_local_file.s);
		p->info_or_local_file.s = NULL;
	}
	DBG_VOID_RETURN;