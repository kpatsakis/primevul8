static bool init_hdr(struct MACH0_(obj_t) *bin) {
	ut8 magicbytes[4] = {0};
	ut8 machohdrbytes[sizeof (struct MACH0_(mach_header))] = {0};
	int len;

	if (r_buf_read_at (bin->b, 0 + bin->header_at, magicbytes, 4) < 1) {
		return false;
	}
	if (r_read_le32 (magicbytes) == 0xfeedface) {
		bin->big_endian = false;
	} else if (r_read_be32 (magicbytes) == 0xfeedface) {
		bin->big_endian = true;
	} else if (r_read_le32 (magicbytes) == FAT_MAGIC) {
		bin->big_endian = false;
	} else if (r_read_be32 (magicbytes) == FAT_MAGIC) {
		bin->big_endian = true;
	} else if (r_read_le32 (magicbytes) == 0xfeedfacf) {
		bin->big_endian = false;
	} else if (r_read_be32 (magicbytes) == 0xfeedfacf) {
		bin->big_endian = true;
	} else {
		return false; // object files are magic == 0, but body is different :?
	}
	len = r_buf_read_at (bin->b, 0 + bin->header_at, machohdrbytes, sizeof (machohdrbytes));
	if (len != sizeof (machohdrbytes)) {
		bprintf ("Error: read (hdr)\n");
		return false;
	}
	bin->hdr.magic = r_read_ble (&machohdrbytes[0], bin->big_endian, 32);
	bin->hdr.cputype = r_read_ble (&machohdrbytes[4], bin->big_endian, 32);
	bin->hdr.cpusubtype = r_read_ble (&machohdrbytes[8], bin->big_endian, 32);
	bin->hdr.filetype = r_read_ble (&machohdrbytes[12], bin->big_endian, 32);
	bin->hdr.ncmds = r_read_ble (&machohdrbytes[16], bin->big_endian, 32);
	bin->hdr.sizeofcmds = r_read_ble (&machohdrbytes[20], bin->big_endian, 32);
	bin->hdr.flags = r_read_ble (&machohdrbytes[24], bin->big_endian, 32);
#if R_BIN_MACH064
	bin->hdr.reserved = r_read_ble (&machohdrbytes[28], bin->big_endian, 32);
#endif
	init_sdb_formats (bin);
	sdb_num_set (bin->kv, "mach0_header.offset", 0, 0); // wat about fatmach0?
	return true;
}