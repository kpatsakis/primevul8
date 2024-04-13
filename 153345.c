findname(dns_name_t **foundname, dns_name_t *target,
	 dns_namelist_t *section)
{
	dns_name_t *curr;

	for (curr = ISC_LIST_TAIL(*section);
	     curr != NULL;
	     curr = ISC_LIST_PREV(curr, link)) {
		if (dns_name_equal(curr, target)) {
			if (foundname != NULL)
				*foundname = curr;
			return (ISC_R_SUCCESS);
		}
	}

	return (ISC_R_NOTFOUND);
}