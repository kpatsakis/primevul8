innodb_switch_mapping(
/*==================*/
	ENGINE_HANDLE*		handle,		/*!< in: Engine handle */
	const void*		cookie,		/*!< in: connection cookie */
	const char*		name,		/*!< in: full name contains
						table map name, and possible
						key value */
	size_t*			name_len,	/*!< in/out: name length,
						out with length excludes
						the table map name */
	bool			has_prefix)	/*!< in: whether the name has
						"@@" prefix */
{
	struct innodb_engine*	innodb_eng = innodb_handle(handle);
	innodb_conn_data_t*	conn_data;
	char			new_name[KEY_MAX_LENGTH];
	meta_cfg_info_t*	meta_info = innodb_eng->meta_info;
	char*			new_map_name;
	unsigned int		new_map_name_len = 0;
	char*			last;
	meta_cfg_info_t*	new_meta_info;
	int			sep_len = 0;

	if (has_prefix) {
		char*		sep = NULL;

		assert(*name_len > 2 && name[0] == '@' && name[1] == '@');
		assert(*name_len < KEY_MAX_LENGTH);

		memcpy(new_name, &name[2], (*name_len) - 2);

		new_name[*name_len - 2] = 0;

		GET_OPTION(meta_info, OPTION_ID_TBL_MAP_SEP, sep, sep_len);

		assert(sep_len > 0);

		new_map_name = strtok_r(new_name, sep, &last);

		if (new_map_name == NULL) {
			return(ENGINE_KEY_ENOENT);
		}

		new_map_name_len = strlen(new_map_name);
	} else {
		/* This is used in the "bind" command, and without the
		"@@" prefix. */
		if (name == NULL) {
			return(ENGINE_KEY_ENOENT);
		}

		new_map_name = (char*) name;
		new_map_name_len = *name_len;
	}

	conn_data = innodb_eng->server.cookie->get_engine_specific(cookie);

	/* Check if we are getting the same configure setting as existing one */
	if (conn_data && conn_data->conn_meta
	    && (new_map_name_len
		== conn_data->conn_meta->col_info[CONTAINER_NAME].col_name_len)
	    && (strcmp(
		new_map_name,
		conn_data->conn_meta->col_info[CONTAINER_NAME].col_name) == 0)) {
		goto get_key_name;
	}

	new_meta_info = innodb_config(
		new_map_name, new_map_name_len, &innodb_eng->meta_hash);

	if (!new_meta_info) {
		return(ENGINE_KEY_ENOENT);
	}

	/* Clean up the existing connection metadata if exists */
	if (conn_data) {
		innodb_conn_clean_data(conn_data, false, false);

		/* Point to the new metadata */
		conn_data->conn_meta = new_meta_info;
	}

	conn_data = innodb_conn_init(innodb_eng, cookie,
				     CONN_MODE_NONE, 0, false,
				     new_meta_info);

	assert(conn_data->conn_meta == new_meta_info);

get_key_name:
	/* Now calculate name length exclude the table mapping name,
	this is the length for the remaining key portion */
	if (has_prefix) {
		assert(*name_len >= strlen(new_map_name) + 2);

		if (*name_len >= strlen(new_map_name) + 2 + sep_len) {
			*name_len -= strlen(new_map_name) + 2 + sep_len;
		} else {
			/* the name does not even contain a delimiter,
			so there will be no keys either */
			*name_len  = 0;
		}
	}

	return(ENGINE_SUCCESS);
}