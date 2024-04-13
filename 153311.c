msgblock_free(isc_mem_t *mctx, dns_msgblock_t *block, unsigned int sizeof_type)
{
	unsigned int length;

	length = sizeof(dns_msgblock_t) + (sizeof_type * block->count);

	isc_mem_put(mctx, block, length);
}