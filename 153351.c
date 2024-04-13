renderset(dns_rdataset_t *rdataset, dns_name_t *owner_name,
	  dns_compress_t *cctx, isc_buffer_t *target,
	  unsigned int reserved, unsigned int options, unsigned int *countp)
{
	isc_result_t result;

	/*
	 * Shrink the space in the buffer by the reserved amount.
	 */
	if (target->length - target->used < reserved)
		return (ISC_R_NOSPACE);

	target->length -= reserved;
	result = dns_rdataset_towire(rdataset, owner_name,
				     cctx, target, options, countp);
	target->length += reserved;

	return (result);
}