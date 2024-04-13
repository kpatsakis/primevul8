void init_proto_http()
{
	int i;
	char *tmp;
	int msg;

	for (msg = 0; msg < HTTP_ERR_SIZE; msg++) {
		if (!http_err_msgs[msg]) {
			Alert("Internal error: no message defined for HTTP return code %d. Aborting.\n", msg);
			abort();
		}

		http_err_chunks[msg].str = (char *)http_err_msgs[msg];
		http_err_chunks[msg].len = strlen(http_err_msgs[msg]);
	}

	/* initialize the log header encoding map : '{|}"#' should be encoded with
	 * '#' as prefix, as well as non-printable characters ( <32 or >= 127 ).
	 * URL encoding only requires '"', '#' to be encoded as well as non-
	 * printable characters above.
	 */
	memset(hdr_encode_map, 0, sizeof(hdr_encode_map));
	memset(url_encode_map, 0, sizeof(url_encode_map));
	for (i = 0; i < 32; i++) {
		FD_SET(i, hdr_encode_map);
		FD_SET(i, url_encode_map);
	}
	for (i = 127; i < 256; i++) {
		FD_SET(i, hdr_encode_map);
		FD_SET(i, url_encode_map);
	}

	tmp = "\"#{|}";
	while (*tmp) {
		FD_SET(*tmp, hdr_encode_map);
		tmp++;
	}

	tmp = "\"#";
	while (*tmp) {
		FD_SET(*tmp, url_encode_map);
		tmp++;
	}

	/* memory allocations */
	pool2_requri = create_pool("requri", REQURI_LEN, MEM_F_SHARED);
	pool2_uniqueid = create_pool("uniqueid", UNIQUEID_LEN, MEM_F_SHARED);
}