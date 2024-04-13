spacefortsig(dns_tsigkey_t *key, int otherlen) {
	isc_region_t r1, r2;
	unsigned int x;
	isc_result_t result;

	/*
	 * The space required for an TSIG record is:
	 *
	 *	n1 bytes for the name
	 *	2 bytes for the type
	 *	2 bytes for the class
	 *	4 bytes for the ttl
	 *	2 bytes for the rdlength
	 *	n2 bytes for the algorithm name
	 *	6 bytes for the time signed
	 *	2 bytes for the fudge
	 *	2 bytes for the MAC size
	 *	x bytes for the MAC
	 *	2 bytes for the original id
	 *	2 bytes for the error
	 *	2 bytes for the other data length
	 *	y bytes for the other data (at most)
	 * ---------------------------------
	 *     26 + n1 + n2 + x + y bytes
	 */

	dns_name_toregion(&key->name, &r1);
	dns_name_toregion(key->algorithm, &r2);
	if (key->key == NULL)
		x = 0;
	else {
		result = dst_key_sigsize(key->key, &x);
		if (result != ISC_R_SUCCESS)
			x = 0;
	}
	return (26 + r1.length + r2.length + x + otherlen);
}