msgblock_allocate(isc_mem_t *mctx, unsigned int sizeof_type,
		  unsigned int count)
{
	dns_msgblock_t *block;
	unsigned int length;

	length = sizeof(dns_msgblock_t) + (sizeof_type * count);

	block = isc_mem_get(mctx, length);
	if (block == NULL)
		return (NULL);

	block->count = count;
	block->remaining = count;

	ISC_LINK_INIT(block, link);

	return (block);
}