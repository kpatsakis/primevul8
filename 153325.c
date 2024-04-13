newbuffer(dns_message_t *msg, unsigned int size) {
	isc_result_t result;
	isc_buffer_t *dynbuf;

	dynbuf = NULL;
	result = isc_buffer_allocate(msg->mctx, &dynbuf, size);
	if (result != ISC_R_SUCCESS)
		return (ISC_R_NOMEMORY);

	ISC_LIST_APPEND(msg->scratchpad, dynbuf, link);
	return (ISC_R_SUCCESS);
}