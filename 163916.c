ldns_str2rdf_wks(ldns_rdf **rd, const char *str)
{
	uint8_t *bitmap = NULL;
	uint8_t *data;
	int bm_len = 0;

	struct protoent *proto = NULL;
	struct servent *serv = NULL;
	int serv_port;

	ldns_buffer *str_buf;

	char *proto_str = NULL;
	char *lc_proto_str = NULL;
	char *token;
	char *lc_token;
	char *c;
	if(strlen(str) == 0)
		token = LDNS_XMALLOC(char, 50);
	else 	token = LDNS_XMALLOC(char, strlen(str)+2);
	if(!token) return LDNS_STATUS_MEM_ERR;

	str_buf = LDNS_MALLOC(ldns_buffer);
	if(!str_buf) {LDNS_FREE(token); return LDNS_STATUS_MEM_ERR;}
	ldns_buffer_new_frm_data(str_buf, (char *)str, strlen(str));
	if(ldns_buffer_status(str_buf) != LDNS_STATUS_OK) {
		LDNS_FREE(str_buf);
		LDNS_FREE(token);
		return LDNS_STATUS_MEM_ERR;
	}

	while(ldns_bget_token(str_buf, token, "\t\n ", strlen(str)) > 0) {
		if (!proto_str) {
			proto_str = strdup(token);
			lc_proto_str = strdup(token);
			for (c = lc_proto_str; *c; c++) {
				*c = tolower(*c);
			}
			if (!proto_str || !lc_proto_str) {
				free(proto_str);
				free(lc_proto_str);
				LDNS_FREE(bitmap);
				LDNS_FREE(token);
	                        ldns_buffer_free(str_buf);
				return LDNS_STATUS_INVALID_STR;
			}
		} else {
			serv = getservbyname(token, proto_str);
			if (!serv) {
				serv = getservbyname(token, lc_proto_str);
			}
			if (!serv && (lc_token = strdup(token))) {
				for (c = lc_token; *c; c++) {
					*c = tolower(*c);
				}
				serv = getservbyname(lc_token, proto_str);
				if (!serv) {
					serv = getservbyname(lc_token, lc_proto_str);
				}
				free(lc_token);
			}
			if (serv) {
				serv_port = (int) ntohs((uint16_t) serv->s_port);
			} else {
				serv_port = atoi(token);
			}
			if (serv_port / 8 >= bm_len) {
				uint8_t *b2 = LDNS_XREALLOC(bitmap, uint8_t, (serv_port / 8) + 1);
                                if(!b2) {
					LDNS_FREE(bitmap);
				        LDNS_FREE(token);
	                                ldns_buffer_free(str_buf);
				        free(proto_str);
				        free(lc_proto_str);
				        return LDNS_STATUS_INVALID_STR;
                                }
				bitmap = b2;
				/* set to zero to be sure */
				for (; bm_len <= serv_port / 8; bm_len++) {
					bitmap[bm_len] = 0;
				}
			}
			ldns_set_bit(bitmap + (serv_port / 8), 7 - (serv_port % 8), true);
		}
	}

	if (!proto_str || !bitmap) {
		LDNS_FREE(bitmap);
		LDNS_FREE(token);
	        ldns_buffer_free(str_buf);
	        free(proto_str);
	        free(lc_proto_str);
		return LDNS_STATUS_INVALID_STR;
	}

	data = LDNS_XMALLOC(uint8_t, bm_len + 1);
        if(!data) {
	        LDNS_FREE(token);
	        ldns_buffer_free(str_buf);
	        LDNS_FREE(bitmap);
	        free(proto_str);
	        free(lc_proto_str);
	        return LDNS_STATUS_INVALID_STR;
        }
    if (proto_str)
		proto = getprotobyname(proto_str);
    	if (!proto) {
		proto = getprotobyname(lc_proto_str);
	}
	if (proto) {
		data[0] = (uint8_t) proto->p_proto;
	} else if (proto_str) {
		data[0] = (uint8_t) atoi(proto_str);
	}
	memcpy(data + 1, bitmap, (size_t) bm_len);

	*rd = ldns_rdf_new_frm_data(LDNS_RDF_TYPE_WKS, (uint16_t) (bm_len + 1), data);

	LDNS_FREE(data);
	LDNS_FREE(token);
	ldns_buffer_free(str_buf);
	LDNS_FREE(bitmap);
	free(proto_str);
	free(lc_proto_str);
#ifdef HAVE_ENDSERVENT
	endservent();
#endif
#ifdef HAVE_ENDPROTOENT
	endprotoent();
#endif

	if(!*rd) return LDNS_STATUS_MEM_ERR;

	return LDNS_STATUS_OK;
}