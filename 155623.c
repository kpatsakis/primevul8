ns_client_aclmsg(const char *msg, const dns_name_t *name, dns_rdatatype_t type,
		 dns_rdataclass_t rdclass, char *buf, size_t len) {
	char namebuf[DNS_NAME_FORMATSIZE];
	char typebuf[DNS_RDATATYPE_FORMATSIZE];
	char classbuf[DNS_RDATACLASS_FORMATSIZE];

	dns_name_format(name, namebuf, sizeof(namebuf));
	dns_rdatatype_format(type, typebuf, sizeof(typebuf));
	dns_rdataclass_format(rdclass, classbuf, sizeof(classbuf));
	(void)snprintf(buf, len, "%s '%s/%s/%s'", msg, namebuf, typebuf,
		       classbuf);
}