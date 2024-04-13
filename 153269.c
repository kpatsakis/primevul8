msgblock_internalget(dns_msgblock_t *block, unsigned int sizeof_type) {
	void *ptr;

	if (block == NULL || block->remaining == 0)
		return (NULL);

	block->remaining--;

	ptr = (((unsigned char *)block)
	       + sizeof(dns_msgblock_t)
	       + (sizeof_type * block->remaining));

	return (ptr);
}