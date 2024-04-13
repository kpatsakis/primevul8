static void meta_add_fileline(RBinFile *bf, ut64 vaddr, ut32 size, RCoreSymCacheElementFLC *flc) {
	Sdb *s = bf->sdb_addrinfo;
	if (!s) {
		return;
	}
	char aoffset[64];
	ut64 cursor = vaddr;
	ut64 end = cursor + R_MAX (size, 1);
	char *fileline = r_str_newf ("%s:%d", flc->file, flc->line);
	while (cursor < end) {
		char *aoffsetptr = sdb_itoa (cursor, aoffset, 16);
		if (!aoffsetptr) {
			break;
		}
		sdb_set (s, aoffsetptr, fileline, 0);
		sdb_set (s, fileline, aoffsetptr, 0);
		cursor += 2;
	}
	free (fileline);
}