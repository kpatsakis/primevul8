wrong_priority(dns_rdataset_t *rds, int pass, dns_rdatatype_t preferred_glue) {
	int pass_needed;

	/*
	 * If we are not rendering class IN, this ordering is bogus.
	 */
	if (rds->rdclass != dns_rdataclass_in)
		return (false);

	switch (rds->type) {
	case dns_rdatatype_a:
	case dns_rdatatype_aaaa:
		if (preferred_glue == rds->type)
			pass_needed = 4;
		else
			pass_needed = 3;
		break;
	case dns_rdatatype_rrsig:
	case dns_rdatatype_dnskey:
		pass_needed = 2;
		break;
	default:
		pass_needed = 1;
	}

	if (pass_needed >= pass)
		return (false);

	return (true);
}