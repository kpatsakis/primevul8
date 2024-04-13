static int ext4_validate_block_bitmap(struct super_block *sb,
				      struct ext4_group_desc *desc,
				      ext4_group_t block_group,
				      struct buffer_head *bh)
{
	ext4_fsblk_t	blk;
	struct ext4_group_info *grp = ext4_get_group_info(sb, block_group);

	if (buffer_verified(bh))
		return 0;
	if (EXT4_MB_GRP_BBITMAP_CORRUPT(grp))
		return -EFSCORRUPTED;

	ext4_lock_group(sb, block_group);
	if (unlikely(!ext4_block_bitmap_csum_verify(sb, block_group,
			desc, bh))) {
		ext4_unlock_group(sb, block_group);
		ext4_error(sb, "bg %u: bad block bitmap checksum", block_group);
		ext4_mark_group_bitmap_corrupted(sb, block_group,
					EXT4_GROUP_INFO_BBITMAP_CORRUPT);
		return -EFSBADCRC;
	}
	blk = ext4_valid_block_bitmap(sb, desc, block_group, bh);
	if (unlikely(blk != 0)) {
		ext4_unlock_group(sb, block_group);
		ext4_error(sb, "bg %u: block %llu: invalid block bitmap",
			   block_group, blk);
		ext4_mark_group_bitmap_corrupted(sb, block_group,
					EXT4_GROUP_INFO_BBITMAP_CORRUPT);
		return -EFSCORRUPTED;
	}
	set_buffer_verified(bh);
	ext4_unlock_group(sb, block_group);
	return 0;
}