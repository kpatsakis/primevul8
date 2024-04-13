static RCoreSymCacheElementHdr *r_coresym_cache_element_header_new(RBuffer *buf, size_t off, int bits) {
	RCoreSymCacheElementHdr *hdr = R_NEW0 (RCoreSymCacheElementHdr);
	if (hdr && r_buf_fread_at (buf, off, (ut8 *)hdr, "13i16c5i", 1) == sizeof (RCoreSymCacheElementHdr)) {
		return hdr;
	}
	free (hdr);
	return NULL;
}