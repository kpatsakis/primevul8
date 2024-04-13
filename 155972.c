static int probe_dos_pt(blkid_probe pr,
		const struct blkid_idmag *mag __attribute__((__unused__)))
{
	int i;
	int ssf;
	blkid_parttable tab = NULL;
	blkid_partlist ls;
	struct dos_partition *p0, *p;
	unsigned char *data;
	uint32_t start, size, id;
	char idstr[37];


	data = blkid_probe_get_sector(pr, 0);
	if (!data) {
		if (errno)
			return -errno;
		goto nothing;
	}

	/* ignore disks with AIX magic number -- for more details see aix.c */
	if (memcmp(data, BLKID_AIX_MAGIC_STRING, BLKID_AIX_MAGIC_STRLEN) == 0)
		goto nothing;

	p0 = mbr_get_partition(data, 0);

	/*
	 * Reject PT where boot indicator is not 0 or 0x80.
	 */
	for (p = p0, i = 0; i < 4; i++, p++)
		if (p->boot_ind != 0 && p->boot_ind != 0x80) {
			DBG(LOWPROBE, ul_debug("missing boot indicator -- ignore"));
			goto nothing;
		}

	/*
	 * GPT uses valid MBR
	 */
	for (p = p0, i = 0; i < 4; i++, p++) {
		if (p->sys_ind == MBR_GPT_PARTITION) {
			DBG(LOWPROBE, ul_debug("probably GPT -- ignore"));
			goto nothing;
		}
	}

	/*
	 * Now that the 55aa signature is present, this is probably
	 * either the boot sector of a FAT filesystem or a DOS-type
	 * partition table.
	 */
	if (blkid_probe_is_vfat(pr) == 1) {
		DBG(LOWPROBE, ul_debug("probably FAT -- ignore"));
		goto nothing;
	}

	blkid_probe_use_wiper(pr, MBR_PT_OFFSET, 512 - MBR_PT_OFFSET);

	id = mbr_get_id(data);
	if (id)
		snprintf(idstr, sizeof(idstr), "%08x", id);

	/*
	 * Well, all checks pass, it's MS-DOS partition table
	 */
	if (blkid_partitions_need_typeonly(pr)) {
		/* Non-binary interface -- caller does not ask for details
		 * about partitions, just set generic variables only. */
		if (id)
			blkid_partitions_strcpy_ptuuid(pr, idstr);
		return 0;
	}

	ls = blkid_probe_get_partlist(pr);
	if (!ls)
		goto nothing;

	/* sector size factor (the start and size are in the real sectors, but
	 * we need to convert all sizes to 512 logical sectors
	 */
	ssf = blkid_probe_get_sectorsize(pr) / 512;

	/* allocate a new partition table */
	tab = blkid_partlist_new_parttable(ls, "dos", MBR_PT_OFFSET);
	if (!tab)
		return -ENOMEM;

	if (id)
		blkid_parttable_set_id(tab, (unsigned char *) idstr);

	/* Parse primary partitions */
	for (p = p0, i = 0; i < 4; i++, p++) {
		blkid_partition par;

		start = dos_partition_get_start(p) * ssf;
		size = dos_partition_get_size(p) * ssf;

		if (!size) {
			/* Linux kernel ignores empty partitions, but partno for
			 * the empty primary partitions is not reused */
			blkid_partlist_increment_partno(ls);
			continue;
		}
		par = blkid_partlist_add_partition(ls, tab, start, size);
		if (!par)
			return -ENOMEM;

		blkid_partition_set_type(par, p->sys_ind);
		blkid_partition_set_flags(par, p->boot_ind);
		blkid_partition_gen_uuid(par);
	}

	/* Linux uses partition numbers greater than 4
	 * for all logical partition and all nested partition tables (bsd, ..)
	 */
	blkid_partlist_set_partno(ls, 5);

	/* Parse logical partitions */
	for (p = p0, i = 0; i < 4; i++, p++) {
		start = dos_partition_get_start(p) * ssf;
		size = dos_partition_get_size(p) * ssf;

		if (!size)
			continue;
		if (is_extended(p) &&
		    parse_dos_extended(pr, tab, start, size, ssf) == -1)
			goto nothing;
	}

	/* Parse subtypes (nested partitions) on large disks */
	if (!blkid_probe_is_tiny(pr)) {
		for (p = p0, i = 0; i < 4; i++, p++) {
			size_t n;
			int rc;

			if (!dos_partition_get_size(p) || is_extended(p))
				continue;

			for (n = 0; n < ARRAY_SIZE(dos_nested); n++) {
				if (dos_nested[n].type != p->sys_ind)
					continue;

				rc = blkid_partitions_do_subprobe(pr,
						blkid_partlist_get_partition(ls, i),
						dos_nested[n].id);
				if (rc < 0)
					return rc;
				break;
			}
		}
	}
	return BLKID_PROBE_OK;

nothing:
	return BLKID_PROBE_NONE;
}