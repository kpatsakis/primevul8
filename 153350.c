msgblock_reset(dns_msgblock_t *block) {
	block->remaining = block->count;
}