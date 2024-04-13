norender_rdataset(const dns_rdataset_t *rdataset, unsigned int options,
		  dns_section_t sectionid)
{
	if (sectionid == DNS_SECTION_QUESTION)
		return (false);

	switch (rdataset->type) {
	case dns_rdatatype_ns:
		if ((options & DNS_MESSAGERENDER_FILTER_AAAA) == 0 ||
		    sectionid != DNS_SECTION_AUTHORITY)
			return (false);
		break;

	case dns_rdatatype_aaaa:
		if ((options & DNS_MESSAGERENDER_FILTER_AAAA) == 0)
			return (false);
		break;

	case dns_rdatatype_rrsig:
		if ((options & DNS_MESSAGERENDER_FILTER_AAAA) == 0 ||
		    (rdataset->covers != dns_rdatatype_ns &&
		     rdataset->covers != dns_rdatatype_aaaa))
			return (false);
		if ((rdataset->covers == dns_rdatatype_ns) &&
		    (sectionid != DNS_SECTION_AUTHORITY))
			return (false);
		break;

	default:
		return (false);
	}

	if (rdataset->rdclass != dns_rdataclass_in)
		return (false);

	return (true);
}