newoffsets(dns_message_t *msg) {
	dns_msgblock_t *msgblock;
	dns_offsets_t *offsets;

	msgblock = ISC_LIST_TAIL(msg->offsets);
	offsets = msgblock_get(msgblock, dns_offsets_t);
	if (offsets == NULL) {
		msgblock = msgblock_allocate(msg->mctx,
					     sizeof(dns_offsets_t),
					     OFFSET_COUNT);
		if (msgblock == NULL)
			return (NULL);

		ISC_LIST_APPEND(msg->offsets, msgblock, link);

		offsets = msgblock_get(msgblock, dns_offsets_t);
	}

	return (offsets);
}