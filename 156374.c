mm_send_keystate(struct monitor *pmonitor)
{
	Buffer m;
	u_char *blob, *p;
	u_int bloblen, plen;
	u_int32_t seqnr, packets;
	u_int64_t blocks;

	buffer_init(&m);

	if (!compat20) {
		u_char iv[24];
		u_char *key;
		u_int ivlen, keylen;

		buffer_put_int(&m, packet_get_protocol_flags());

		buffer_put_int(&m, packet_get_ssh1_cipher());

		debug3("%s: Sending ssh1 KEY+IV", __func__);
		keylen = packet_get_encryption_key(NULL);
		key = xmalloc(keylen+1);	/* add 1 if keylen == 0 */
		keylen = packet_get_encryption_key(key);
		buffer_put_string(&m, key, keylen);
		memset(key, 0, keylen);
		xfree(key);

		ivlen = packet_get_keyiv_len(MODE_OUT);
		packet_get_keyiv(MODE_OUT, iv, ivlen);
		buffer_put_string(&m, iv, ivlen);
		ivlen = packet_get_keyiv_len(MODE_OUT);
		packet_get_keyiv(MODE_IN, iv, ivlen);
		buffer_put_string(&m, iv, ivlen);
		goto skip;
	} else {
		/* Kex for rekeying */
		mm_send_kex(&m, *pmonitor->m_pkex);
	}

	debug3("%s: Sending new keys: %p %p",
	    __func__, newkeys[MODE_OUT], newkeys[MODE_IN]);

	/* Keys from Kex */
	if (!mm_newkeys_to_blob(MODE_OUT, &blob, &bloblen))
		fatal("%s: conversion of newkeys failed", __func__);

	buffer_put_string(&m, blob, bloblen);
	xfree(blob);

	if (!mm_newkeys_to_blob(MODE_IN, &blob, &bloblen))
		fatal("%s: conversion of newkeys failed", __func__);

	buffer_put_string(&m, blob, bloblen);
	xfree(blob);

	packet_get_state(MODE_OUT, &seqnr, &blocks, &packets);
	buffer_put_int(&m, seqnr);
	buffer_put_int64(&m, blocks);
	buffer_put_int(&m, packets);
	packet_get_state(MODE_IN, &seqnr, &blocks, &packets);
	buffer_put_int(&m, seqnr);
	buffer_put_int64(&m, blocks);
	buffer_put_int(&m, packets);

	debug3("%s: New keys have been sent", __func__);
 skip:
	/* More key context */
	plen = packet_get_keycontext(MODE_OUT, NULL);
	p = xmalloc(plen+1);
	packet_get_keycontext(MODE_OUT, p);
	buffer_put_string(&m, p, plen);
	xfree(p);

	plen = packet_get_keycontext(MODE_IN, NULL);
	p = xmalloc(plen+1);
	packet_get_keycontext(MODE_IN, p);
	buffer_put_string(&m, p, plen);
	xfree(p);

	/* Compression state */
	debug3("%s: Sending compression state", __func__);
	buffer_put_string(&m, &outgoing_stream, sizeof(outgoing_stream));
	buffer_put_string(&m, &incoming_stream, sizeof(incoming_stream));

	/* Network I/O buffers */
	buffer_put_string(&m, buffer_ptr(&input), buffer_len(&input));
	buffer_put_string(&m, buffer_ptr(&output), buffer_len(&output));

	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_KEYEXPORT, &m);
	debug3("%s: Finished sending state", __func__);

	buffer_free(&m);
}