static enum_func_status
php_mysqlnd_rset_field_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	/* Should be enough for the metadata of a single row */
	MYSQLND_PACKET_RES_FIELD *packet = (MYSQLND_PACKET_RES_FIELD *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	const size_t buf_len = pfc->cmd_buffer.length;
	size_t total_len = 0;
	zend_uchar * const buf = (zend_uchar *) pfc->cmd_buffer.buffer;
	const zend_uchar * p = buf;
	const zend_uchar * const begin = buf;
	char *root_ptr;
	zend_ulong len;
	MYSQLND_FIELD *meta;

	DBG_ENTER("php_mysqlnd_rset_field_read");

	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, buf_len, "field", PROT_RSET_FLD_PACKET)) {
		DBG_RETURN(FAIL);
	}

	if (packet->skip_parsing) {
		DBG_RETURN(PASS);
	}

	BAIL_IF_NO_MORE_DATA;
	if (ERROR_MARKER == *p) {
		/* Error */
		p++;
		BAIL_IF_NO_MORE_DATA;
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error_info.error, sizeof(packet->error_info.error),
										 &packet->error_info.error_no, packet->error_info.sqlstate
										);
		DBG_ERR_FMT("Server error : (%u) %s", packet->error_info.error_no, packet->error_info.error);
		DBG_RETURN(PASS);
	} else if (EODATA_MARKER == *p && packet->header.size < 8) {
		/* Premature EOF. That should be COM_FIELD_LIST. But we don't support COM_FIELD_LIST anymore, thus this should not happen */
		DBG_ERR("Premature EOF. That should be COM_FIELD_LIST");
		php_error_docref(NULL, E_WARNING, "Premature EOF in result field metadata");
		DBG_RETURN(FAIL);
	}

	meta = packet->metadata;

	READ_RSET_FIELD(catalog);
	READ_RSET_FIELD(db);
	READ_RSET_FIELD(table);
	READ_RSET_FIELD(org_table);
	READ_RSET_FIELD(name);
	READ_RSET_FIELD(org_name);

	/* 1 byte length */
	if (UNEXPECTED(12 != *p)) {
		DBG_ERR_FMT("Protocol error. Server sent false length. Expected 12 got %d", (int) *p);
		php_error_docref(NULL, E_WARNING, "Protocol error. Server sent false length. Expected 12");
	}

	if ((size_t)((p - begin) + 12) > packet->header.size) {
		php_error_docref(NULL, E_WARNING, "Premature end of data (mysqlnd_wireprotocol.c:%u)", __LINE__);
		goto premature_end;
	}

	p++;

	meta->charsetnr = uint2korr(p);
	p += 2;

	meta->length = uint4korr(p);
	p += 4;

	meta->type = uint1korr(p);
	p += 1;

	meta->flags = uint2korr(p);
	p += 2;

	meta->decimals = uint1korr(p);
	p += 1;

	/* 2 byte filler */
	p +=2;

	/* Should we set NUM_FLAG (libmysql does it) ? */
	if (
		(meta->type <= MYSQL_TYPE_INT24 &&
			(meta->type != MYSQL_TYPE_TIMESTAMP || meta->length == 14 || meta->length == 8)
		) || meta->type == MYSQL_TYPE_YEAR)
	{
		meta->flags |= NUM_FLAG;
	}


	/*
	  def could be empty, thus don't allocate on the root.
	  NULL_LENGTH (0xFB) comes from COM_FIELD_LIST when the default value is NULL.
	  Otherwise the string is length encoded.
	*/
	if (packet->header.size > (size_t) (p - buf) &&
		(len = php_mysqlnd_net_field_length(&p)) &&
		len != MYSQLND_NULL_LENGTH)
	{
		BAIL_IF_NO_MORE_DATA;
		DBG_INF_FMT("Def found, length %lu", len);
		meta->def = packet->memory_pool->get_chunk(packet->memory_pool, len + 1);
		memcpy(meta->def, p, len);
		meta->def[len] = '\0';
		meta->def_length = len;
		p += len;
	}

	root_ptr = meta->root = packet->memory_pool->get_chunk(packet->memory_pool, total_len);
	meta->root_len = total_len;

	if (EXPECTED(meta->name_length != 0)) {
		meta->sname = zend_string_init_interned(meta->name, meta->name_length, 0);
		meta->name = ZSTR_VAL(meta->sname);
	} else {
		meta->sname = ZSTR_EMPTY_ALLOC();
	}

	/* Now do allocs */
	if (meta->catalog_length != 0) {
		len = meta->catalog_length;
		meta->catalog = memcpy(root_ptr, meta->catalog, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->db_length != 0) {
		len = meta->db_length;
		meta->db = memcpy(root_ptr, meta->db, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->table_length != 0) {
		len = meta->table_length;
		meta->table = memcpy(root_ptr, meta->table, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->org_table_length != 0) {
		len = meta->org_table_length;
		meta->org_table = memcpy(root_ptr, meta->org_table, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->org_name_length != 0) {
		len = meta->org_name_length;
		meta->org_name = memcpy(root_ptr, meta->org_name, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	DBG_INF_FMT("allocing root.");

	DBG_INF_FMT("FIELD=[%s.%s.%s]", meta->db? meta->db:"*NA*", meta->table? meta->table:"*NA*",
				meta->name? meta->name:"*NA*");

	DBG_RETURN(PASS);

faulty_or_fake:
	DBG_ERR_FMT("Protocol error. Server sent NULL_LENGTH. The server is faulty");
	php_error_docref(NULL, E_WARNING, "Protocol error. Server sent NULL_LENGTH."
					 " The server is faulty");
	DBG_RETURN(FAIL);
premature_end:
	DBG_ERR_FMT("RSET field packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "Result set field packet "MYSQLND_SZ_T_SPEC" bytes "
			 		"shorter than expected", p - begin - packet->header.size);
	DBG_RETURN(FAIL);