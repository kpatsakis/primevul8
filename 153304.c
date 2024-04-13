dns_message_create(isc_mem_t *mctx, unsigned int intent, dns_message_t **msgp)
{
	dns_message_t *m;
	isc_result_t result;
	isc_buffer_t *dynbuf;
	unsigned int i;

	REQUIRE(mctx != NULL);
	REQUIRE(msgp != NULL);
	REQUIRE(*msgp == NULL);
	REQUIRE(intent == DNS_MESSAGE_INTENTPARSE
		|| intent == DNS_MESSAGE_INTENTRENDER);

	m = isc_mem_get(mctx, sizeof(dns_message_t));
	if (m == NULL)
		return (ISC_R_NOMEMORY);

	/*
	 * No allocations until further notice.  Just initialize all lists
	 * and other members that are freed in the cleanup phase here.
	 */

	m->magic = DNS_MESSAGE_MAGIC;
	m->from_to_wire = intent;
	msginit(m);

	for (i = 0; i < DNS_SECTION_MAX; i++)
		ISC_LIST_INIT(m->sections[i]);

	m->mctx = NULL;
	isc_mem_attach(mctx, &m->mctx);

	ISC_LIST_INIT(m->scratchpad);
	ISC_LIST_INIT(m->cleanup);
	m->namepool = NULL;
	m->rdspool = NULL;
	ISC_LIST_INIT(m->rdatas);
	ISC_LIST_INIT(m->rdatalists);
	ISC_LIST_INIT(m->offsets);
	ISC_LIST_INIT(m->freerdata);
	ISC_LIST_INIT(m->freerdatalist);

	/*
	 * Ok, it is safe to allocate (and then "goto cleanup" if failure)
	 */

	result = isc_mempool_create(m->mctx, sizeof(dns_name_t), &m->namepool);
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	isc_mempool_setfillcount(m->namepool, NAME_COUNT);
	isc_mempool_setfreemax(m->namepool, NAME_COUNT);
	isc_mempool_setname(m->namepool, "msg:names");

	result = isc_mempool_create(m->mctx, sizeof(dns_rdataset_t),
				    &m->rdspool);
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	isc_mempool_setfillcount(m->rdspool, RDATASET_COUNT);
	isc_mempool_setfreemax(m->rdspool, RDATASET_COUNT);
	isc_mempool_setname(m->rdspool, "msg:rdataset");

	dynbuf = NULL;
	result = isc_buffer_allocate(mctx, &dynbuf, SCRATCHPAD_SIZE);
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	ISC_LIST_APPEND(m->scratchpad, dynbuf, link);

	m->cctx = NULL;

	*msgp = m;
	return (ISC_R_SUCCESS);

	/*
	 * Cleanup for error returns.
	 */
 cleanup:
	dynbuf = ISC_LIST_HEAD(m->scratchpad);
	if (dynbuf != NULL) {
		ISC_LIST_UNLINK(m->scratchpad, dynbuf, link);
		isc_buffer_free(&dynbuf);
	}
	if (m->namepool != NULL)
		isc_mempool_destroy(&m->namepool);
	if (m->rdspool != NULL)
		isc_mempool_destroy(&m->rdspool);
	m->magic = 0;
	isc_mem_putanddetach(&mctx, m, sizeof(dns_message_t));

	return (ISC_R_NOMEMORY);
}