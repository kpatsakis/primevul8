update(dns_section_t section, dns_rdataclass_t rdclass) {
	if (section == DNS_SECTION_PREREQUISITE)
		return (rdclass == dns_rdataclass_any ||
			 rdclass == dns_rdataclass_none);
	if (section == DNS_SECTION_UPDATE)
		return (rdclass == dns_rdataclass_any);
	return (false);
}