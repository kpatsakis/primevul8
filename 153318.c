auth_signed(dns_namelist_t *section) {
	dns_name_t *name;

	for (name = ISC_LIST_HEAD(*section);
	     name != NULL;
	     name = ISC_LIST_NEXT(name, link))
	{
		int auth_dnssec = 0, auth_rrsig = 0;
		dns_rdataset_t *rds;

		for (rds = ISC_LIST_HEAD(name->list);
		     rds != NULL;
		     rds = ISC_LIST_NEXT(rds, link))
		{
			switch (rds->type) {
			case dns_rdatatype_ds:
				auth_dnssec |= 0x1;
				break;
			case dns_rdatatype_nsec:
				auth_dnssec |= 0x2;
				break;
			case dns_rdatatype_nsec3:
				auth_dnssec |= 0x4;
				break;
			case dns_rdatatype_rrsig:
				break;
			default:
				continue;
			}

			switch (rds->covers) {
			case dns_rdatatype_ds:
				auth_rrsig |= 0x1;
				break;
			case dns_rdatatype_nsec:
				auth_rrsig |= 0x2;
				break;
			case dns_rdatatype_nsec3:
				auth_rrsig |= 0x4;
				break;
			default:
				break;
			}
		}

		if (auth_dnssec != auth_rrsig)
			return (false);
	}

	return (true);
}