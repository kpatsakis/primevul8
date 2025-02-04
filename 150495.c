static bool parse_symtab(struct MACH0_(obj_t) *mo, ut64 off) {
	struct symtab_command st;
	ut32 size_sym;
	size_t i;
	const char *error_message = "";
	ut8 symt[sizeof (struct symtab_command)] = {0};
	ut8 nlst[sizeof (struct MACH0_(nlist))] = {0};
	const bool be = mo->big_endian;

	if (off > (ut64)mo->size || off + sizeof (struct symtab_command) > (ut64)mo->size) {
		return false;
	}
	int len = r_buf_read_at (mo->b, off, symt, sizeof (struct symtab_command));
	if (len != sizeof (struct symtab_command)) {
		Eprintf ("Error: read (symtab)\n");
		return false;
	}
	st.cmd = r_read_ble32 (symt, be);
	st.cmdsize = r_read_ble32 (symt + 4, be);
	st.symoff = r_read_ble32 (symt + 8, be) + mo->symbols_off;
	st.nsyms = r_read_ble32 (symt + 12, be);
	st.stroff = r_read_ble32 (symt + 16, be) + mo->symbols_off;
	st.strsize = r_read_ble32 (symt + 20, be);

	mo->symtab = NULL;
	mo->nsymtab = 0;
	if (st.strsize > 0 && st.strsize < mo->size && st.nsyms > 0) {
		mo->nsymtab = st.nsyms;
		if (st.stroff > mo->size || st.stroff + st.strsize > mo->size) {
			Error ("fail");
		}
		if (!UT32_MUL (&size_sym, mo->nsymtab, sizeof (struct MACH0_(nlist)))) {
			Error ("fail2");
		}
		if (!size_sym) {
			Error ("symbol size is zero");
		}
		if (st.symoff > mo->size || st.symoff + size_sym > mo->size) {
			Error ("symoff is out of bounds");
		}
		if (!(mo->symstr = calloc (1, st.strsize + 2))) {
			Error ("symoff is out of bounds");
		}
		mo->symstrlen = st.strsize;
		len = r_buf_read_at (mo->b, st.stroff, (ut8*)mo->symstr, st.strsize);
		if (len != st.strsize) {
			Error ("Error: read (symstr)");
		}
		ut64 max_nsymtab = (r_buf_size (mo->b) - st.symoff) / sizeof (struct MACH0_(nlist));
		if (mo->nsymtab > max_nsymtab || !(mo->symtab = calloc (mo->nsymtab, sizeof (struct MACH0_(nlist))))) {
			goto error;
		}
		for (i = 0; i < mo->nsymtab; i++) {
			ut64 at = st.symoff + (i * sizeof (struct MACH0_(nlist)));
			len = r_buf_read_at (mo->b, at, nlst, sizeof (struct MACH0_(nlist)));
			if (len != sizeof (struct MACH0_(nlist))) {
				Error ("read (nlist)");
			}
			//XXX not very safe what if is n_un.n_name instead?
			mo->symtab[i].n_strx = r_read_ble32 (nlst, be);
			mo->symtab[i].n_type = r_read_ble8 (nlst + 4);
			mo->symtab[i].n_sect = r_read_ble8 (nlst + 5);
			mo->symtab[i].n_desc = r_read_ble16 (nlst + 6, be);
#if R_BIN_MACH064
			mo->symtab[i].n_value = r_read_ble64 (&nlst[8], be);
#else
			mo->symtab[i].n_value = r_read_ble32 (&nlst[8], be);
#endif
		}
	}
	return true;
error:
	R_FREE (mo->symstr);
	R_FREE (mo->symtab);
	Eprintf ("%s\n", error_message);
	return false;
}