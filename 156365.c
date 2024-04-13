monitor_apply_keystate(struct monitor *pmonitor)
{
	if (compat20) {
		set_newkeys(MODE_IN);
		set_newkeys(MODE_OUT);
	} else {
		packet_set_protocol_flags(child_state.ssh1protoflags);
		packet_set_encryption_key(child_state.ssh1key,
		    child_state.ssh1keylen, child_state.ssh1cipher);
		xfree(child_state.ssh1key);
	}

	/* for rc4 and other stateful ciphers */
	packet_set_keycontext(MODE_OUT, child_state.keyout);
	xfree(child_state.keyout);
	packet_set_keycontext(MODE_IN, child_state.keyin);
	xfree(child_state.keyin);

	if (!compat20) {
		packet_set_iv(MODE_OUT, child_state.ivout);
		xfree(child_state.ivout);
		packet_set_iv(MODE_IN, child_state.ivin);
		xfree(child_state.ivin);
	}

	memcpy(&incoming_stream, &child_state.incoming,
	    sizeof(incoming_stream));
	memcpy(&outgoing_stream, &child_state.outgoing,
	    sizeof(outgoing_stream));

	/* Update with new address */
	if (options.compression)
		mm_init_compression(pmonitor->m_zlib);

	/* Network I/O buffers */
	/* XXX inefficient for large buffers, need: buffer_init_from_string */
	buffer_clear(&input);
	buffer_append(&input, child_state.input, child_state.ilen);
	memset(child_state.input, 0, child_state.ilen);
	xfree(child_state.input);

	buffer_clear(&output);
	buffer_append(&output, child_state.output, child_state.olen);
	memset(child_state.output, 0, child_state.olen);
	xfree(child_state.output);
}