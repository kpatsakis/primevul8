static void
php_mysqlnd_rowp_free_mem(void * _packet)
{
	MYSQLND_PACKET_ROW *p;

	DBG_ENTER("php_mysqlnd_rowp_free_mem");
	p = (MYSQLND_PACKET_ROW *) _packet;
	if (p->row_buffer.ptr) {
		p->result_set_memory_pool->free_chunk(p->result_set_memory_pool, p->row_buffer.ptr);
		p->row_buffer.ptr = NULL;
	}
	/*
	  Don't free packet->fields :
	  - normal queries -> store_result() | fetch_row_unbuffered() will transfer
	    the ownership and NULL it.
	  - PS will pass in it the bound variables, we have to use them! and of course
	    not free the array. As it is passed to us, we should not clean it ourselves.
	*/
	DBG_VOID_RETURN;