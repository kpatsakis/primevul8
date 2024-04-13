mm_get_kex(Buffer *m)
{
	Kex *kex;
	void *blob;
	u_int bloblen;

	kex = xmalloc(sizeof(*kex));
	memset(kex, 0, sizeof(*kex));
	kex->session_id = buffer_get_string(m, &kex->session_id_len);
	if ((session_id2 == NULL) ||
	    (kex->session_id_len != session_id2_len) ||
	    (memcmp(kex->session_id, session_id2, session_id2_len) != 0))
		fatal("mm_get_get: internal error: bad session id");
	kex->we_need = buffer_get_int(m);
	kex->kex[KEX_DH_GRP1_SHA1] = kexdh_server;
	kex->kex[KEX_DH_GEX_SHA1] = kexgex_server;
	kex->server = 1;
	kex->hostkey_type = buffer_get_int(m);
	kex->kex_type = buffer_get_int(m);
	blob = buffer_get_string(m, &bloblen);
	buffer_init(&kex->my);
	buffer_append(&kex->my, blob, bloblen);
	xfree(blob);
	blob = buffer_get_string(m, &bloblen);
	buffer_init(&kex->peer);
	buffer_append(&kex->peer, blob, bloblen);
	xfree(blob);
	kex->done = 1;
	kex->flags = buffer_get_int(m);
	kex->client_version_string = buffer_get_string(m, NULL);
	kex->server_version_string = buffer_get_string(m, NULL);
	kex->load_host_key=&get_hostkey_by_type;
	kex->host_key_index=&get_hostkey_index;

	return (kex);
}