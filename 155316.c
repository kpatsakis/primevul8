static inline int resolve_named_entity_html(const char *start, size_t length, const entity_ht *ht, unsigned *uni_cp1, unsigned *uni_cp2)
{
	const entity_cp_map *s;
	ulong hash = zend_inline_hash_func(start, length);

	s = ht->buckets[hash % ht->num_elems];
	while (s->entity) {
		if (s->entity_len == length) {
			if (memcmp(start, s->entity, length) == 0) {
				*uni_cp1 = s->codepoint1;
				*uni_cp2 = s->codepoint2;
				return SUCCESS;
			}
		}
		s++;
	}
	return FAILURE;
}