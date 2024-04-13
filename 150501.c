static int reloc_comparator(struct reloc_t *a, struct reloc_t *b) {
	return a->addr - b->addr;
}