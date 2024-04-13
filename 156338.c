mm_send_kex(Buffer *m, Kex *kex)
{
	buffer_put_string(m, kex->session_id, kex->session_id_len);
	buffer_put_int(m, kex->we_need);
	buffer_put_int(m, kex->hostkey_type);
	buffer_put_int(m, kex->kex_type);
	buffer_put_string(m, buffer_ptr(&kex->my), buffer_len(&kex->my));
	buffer_put_string(m, buffer_ptr(&kex->peer), buffer_len(&kex->peer));
	buffer_put_int(m, kex->flags);
	buffer_put_cstring(m, kex->client_version_string);
	buffer_put_cstring(m, kex->server_version_string);
}