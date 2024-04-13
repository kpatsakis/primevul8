mm_get_keystate(struct monitor *pmonitor)
{
	Buffer m;
	u_char *blob, *p;
	u_int bloblen, plen;
	u_int32_t seqnr, packets;
	u_int64_t blocks;

	debug3("%s: Waiting for new keys", __func__);

	buffer_init(&m);
	mm_request_receive_expect(pmonitor->m_sendfd, MONITOR_REQ_KEYEXPORT, &m);
	if (!compat20) {
		child_state.ssh1protoflags = buffer_get_int(&m);
		child_state.ssh1cipher = buffer_get_int(&m);
		child_state.ssh1key = buffer_get_string(&m,
		    &child_state.ssh1keylen);
		child_state.ivout = buffer_get_string(&m,
		    &child_state.ivoutlen);
		child_state.ivin = buffer_get_string(&m, &child_state.ivinlen);
		goto skip;
	} else {
		/* Get the Kex for rekeying */
		*pmonitor->m_pkex = mm_get_kex(&m);
	}

	blob = buffer_get_string(&m, &bloblen);
	current_keys[MODE_OUT] = mm_newkeys_from_blob(blob, bloblen);
	xfree(blob);

	debug3("%s: Waiting for second key", __func__);
	blob = buffer_get_string(&m, &bloblen);
	current_keys[MODE_IN] = mm_newkeys_from_blob(blob, bloblen);
	xfree(blob);

	/* Now get sequence numbers for the packets */
	seqnr = buffer_get_int(&m);
	blocks = buffer_get_int64(&m);
	packets = buffer_get_int(&m);
	packet_set_state(MODE_OUT, seqnr, blocks, packets);
	seqnr = buffer_get_int(&m);
	blocks = buffer_get_int64(&m);
	packets = buffer_get_int(&m);
	packet_set_state(MODE_IN, seqnr, blocks, packets);

 skip:
	/* Get the key context */
	child_state.keyout = buffer_get_string(&m, &child_state.keyoutlen);
	child_state.keyin  = buffer_get_string(&m, &child_state.keyinlen);

	debug3("%s: Getting compression state", __func__);
	/* Get compression state */
	p = buffer_get_string(&m, &plen);
	if (plen != sizeof(child_state.outgoing))
		fatal("%s: bad request size", __func__);
	memcpy(&child_state.outgoing, p, sizeof(child_state.outgoing));
	xfree(p);

	p = buffer_get_string(&m, &plen);
	if (plen != sizeof(child_state.incoming))
		fatal("%s: bad request size", __func__);
	memcpy(&child_state.incoming, p, sizeof(child_state.incoming));
	xfree(p);

	/* Network I/O buffers */
	debug3("%s: Getting Network I/O buffers", __func__);
	child_state.input = buffer_get_string(&m, &child_state.ilen);
	child_state.output = buffer_get_string(&m, &child_state.olen);

	buffer_free(&m);
}