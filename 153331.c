msgresetnames(dns_message_t *msg, unsigned int first_section) {
	unsigned int i;
	dns_name_t *name, *next_name;
	dns_rdataset_t *rds, *next_rds;

	/*
	 * Clean up name lists by calling the rdataset disassociate function.
	 */
	for (i = first_section; i < DNS_SECTION_MAX; i++) {
		name = ISC_LIST_HEAD(msg->sections[i]);
		while (name != NULL) {
			next_name = ISC_LIST_NEXT(name, link);
			ISC_LIST_UNLINK(msg->sections[i], name, link);

			rds = ISC_LIST_HEAD(name->list);
			while (rds != NULL) {
				next_rds = ISC_LIST_NEXT(rds, link);
				ISC_LIST_UNLINK(name->list, rds, link);

				INSIST(dns_rdataset_isassociated(rds));
				dns_rdataset_disassociate(rds);
				isc_mempool_put(msg->rdspool, rds);
				rds = next_rds;
			}
			if (dns_name_dynamic(name))
				dns_name_free(name, msg->mctx);
			isc_mempool_put(msg->namepool, name);
			name = next_name;
		}
	}
}