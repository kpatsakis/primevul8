dns_message_find(dns_name_t *name, dns_rdataclass_t rdclass,
		 dns_rdatatype_t type, dns_rdatatype_t covers,
		 dns_rdataset_t **rdataset)
{
	dns_rdataset_t *curr;

	REQUIRE(name != NULL);
	REQUIRE(rdataset == NULL || *rdataset == NULL);

	for (curr = ISC_LIST_TAIL(name->list);
	     curr != NULL;
	     curr = ISC_LIST_PREV(curr, link)) {
		if (curr->rdclass == rdclass &&
		    curr->type == type && curr->covers == covers) {
			if (rdataset != NULL)
				*rdataset = curr;
			return (ISC_R_SUCCESS);
		}
	}

	return (ISC_R_NOTFOUND);
}