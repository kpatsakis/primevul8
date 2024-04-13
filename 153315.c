msgreset(dns_message_t *msg, bool everything) {
	dns_msgblock_t *msgblock, *next_msgblock;
	isc_buffer_t *dynbuf, *next_dynbuf;
	dns_rdata_t *rdata;
	dns_rdatalist_t *rdatalist;

	msgresetnames(msg, 0);
	msgresetopt(msg);
	msgresetsigs(msg, false);

	/*
	 * Clean up linked lists.
	 */

	/*
	 * Run through the free lists, and just unlink anything found there.
	 * The memory isn't lost since these are part of message blocks we
	 * have allocated.
	 */
	rdata = ISC_LIST_HEAD(msg->freerdata);
	while (rdata != NULL) {
		ISC_LIST_UNLINK(msg->freerdata, rdata, link);
		rdata = ISC_LIST_HEAD(msg->freerdata);
	}
	rdatalist = ISC_LIST_HEAD(msg->freerdatalist);
	while (rdatalist != NULL) {
		ISC_LIST_UNLINK(msg->freerdatalist, rdatalist, link);
		rdatalist = ISC_LIST_HEAD(msg->freerdatalist);
	}

	dynbuf = ISC_LIST_HEAD(msg->scratchpad);
	INSIST(dynbuf != NULL);
	if (!everything) {
		isc_buffer_clear(dynbuf);
		dynbuf = ISC_LIST_NEXT(dynbuf, link);
	}
	while (dynbuf != NULL) {
		next_dynbuf = ISC_LIST_NEXT(dynbuf, link);
		ISC_LIST_UNLINK(msg->scratchpad, dynbuf, link);
		isc_buffer_free(&dynbuf);
		dynbuf = next_dynbuf;
	}

	msgblock = ISC_LIST_HEAD(msg->rdatas);
	if (!everything && msgblock != NULL) {
		msgblock_reset(msgblock);
		msgblock = ISC_LIST_NEXT(msgblock, link);
	}
	while (msgblock != NULL) {
		next_msgblock = ISC_LIST_NEXT(msgblock, link);
		ISC_LIST_UNLINK(msg->rdatas, msgblock, link);
		msgblock_free(msg->mctx, msgblock, sizeof(dns_rdata_t));
		msgblock = next_msgblock;
	}

	/*
	 * rdatalists could be empty.
	 */

	msgblock = ISC_LIST_HEAD(msg->rdatalists);
	if (!everything && msgblock != NULL) {
		msgblock_reset(msgblock);
		msgblock = ISC_LIST_NEXT(msgblock, link);
	}
	while (msgblock != NULL) {
		next_msgblock = ISC_LIST_NEXT(msgblock, link);
		ISC_LIST_UNLINK(msg->rdatalists, msgblock, link);
		msgblock_free(msg->mctx, msgblock, sizeof(dns_rdatalist_t));
		msgblock = next_msgblock;
	}

	msgblock = ISC_LIST_HEAD(msg->offsets);
	if (!everything && msgblock != NULL) {
		msgblock_reset(msgblock);
		msgblock = ISC_LIST_NEXT(msgblock, link);
	}
	while (msgblock != NULL) {
		next_msgblock = ISC_LIST_NEXT(msgblock, link);
		ISC_LIST_UNLINK(msg->offsets, msgblock, link);
		msgblock_free(msg->mctx, msgblock, sizeof(dns_offsets_t));
		msgblock = next_msgblock;
	}

	if (msg->tsigkey != NULL) {
		dns_tsigkey_detach(&msg->tsigkey);
		msg->tsigkey = NULL;
	}

	if (msg->tsigctx != NULL)
		dst_context_destroy(&msg->tsigctx);

	if (msg->query.base != NULL) {
		if (msg->free_query != 0)
			isc_mem_put(msg->mctx, msg->query.base,
				    msg->query.length);
		msg->query.base = NULL;
		msg->query.length = 0;
	}

	if (msg->saved.base != NULL) {
		if (msg->free_saved != 0)
			isc_mem_put(msg->mctx, msg->saved.base,
				    msg->saved.length);
		msg->saved.base = NULL;
		msg->saved.length = 0;
	}

	/*
	 * cleanup the buffer cleanup list
	 */
	dynbuf = ISC_LIST_HEAD(msg->cleanup);
	while (dynbuf != NULL) {
		next_dynbuf = ISC_LIST_NEXT(dynbuf, link);
		ISC_LIST_UNLINK(msg->cleanup, dynbuf, link);
		isc_buffer_free(&dynbuf);
		dynbuf = next_dynbuf;
	}

	/*
	 * Set other bits to normal default values.
	 */
	if (!everything)
		msginit(msg);

	ENSURE(isc_mempool_getallocated(msg->namepool) == 0);
	ENSURE(isc_mempool_getallocated(msg->rdspool) == 0);
}