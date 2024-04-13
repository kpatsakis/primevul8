PgDatabase *register_auto_database(const char *name)
{
	PgDatabase *db;
	int len;
	char *cs;
	
	if (!cf_autodb_connstr)
		return NULL;

	len = strlen(cf_autodb_connstr);
	cs = malloc(len + 1);
	if (!cs)
		return NULL;
	memcpy(cs, cf_autodb_connstr, len + 1);
	parse_database(NULL, (char*)name, cs);
	free(cs);

	db = find_database(name);
	if (db) {
		db->db_auto = 1;
		/* do not forget to check pool_size like in config_postprocess */
		if (db->pool_size < 0)
			db->pool_size = cf_default_pool_size;
		if (db->res_pool_size < 0)
			db->res_pool_size = cf_res_pool_size;
	}

	return db;
}