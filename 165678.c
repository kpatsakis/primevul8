static int get_sg_io_pack_id(int *pack_id, void __user *buf, size_t count)
{
	struct sg_header __user *old_hdr = buf;
	int reply_len;

	if (count >= SZ_SG_HEADER) {
		/* negative reply_len means v3 format, otherwise v1/v2 */
		if (get_user(reply_len, &old_hdr->reply_len))
			return -EFAULT;

		if (reply_len >= 0)
			return get_user(*pack_id, &old_hdr->pack_id);

		if (in_compat_syscall() &&
		    count >= sizeof(struct compat_sg_io_hdr)) {
			struct compat_sg_io_hdr __user *hp = buf;

			return get_user(*pack_id, &hp->pack_id);
		}

		if (count >= sizeof(struct sg_io_hdr)) {
			struct sg_io_hdr __user *hp = buf;

			return get_user(*pack_id, &hp->pack_id);
		}
	}

	/* no valid header was passed, so ignore the pack_id */
	*pack_id = -1;
	return 0;
}