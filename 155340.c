static inline void find_entity_for_char_basic(
	unsigned int k,
	const entity_stage3_row *table,
	const unsigned char **entity,
	size_t *entity_len)
{
	if (k >= 64U) {
		*entity     = NULL;
		*entity_len = 0;
		return;
	}

	*entity     = table[k].data.ent.entity;
	*entity_len = table[k].data.ent.entity_len;
}