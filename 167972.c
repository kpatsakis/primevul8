ext4_ext_put_gap_in_cache(struct inode *inode, ext4_lblk_t hole_start,
			  ext4_lblk_t hole_len)
{
	struct extent_status es;

	ext4_es_find_delayed_extent_range(inode, hole_start,
					  hole_start + hole_len - 1, &es);
	if (es.es_len) {
		/* There's delayed extent containing lblock? */
		if (es.es_lblk <= hole_start)
			return;
		hole_len = min(es.es_lblk - hole_start, hole_len);
	}
	ext_debug(" -> %u:%u\n", hole_start, hole_len);
	ext4_es_insert_extent(inode, hole_start, hole_len, ~0,
			      EXTENT_STATUS_HOLE);
}