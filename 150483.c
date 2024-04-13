RList *MACH0_(mach_fields)(RBinFile *bf) {
	RBuffer *buf = bf->buf;
	ut64 length = r_buf_size (buf);
	struct MACH0_(mach_header) *mh = MACH0_(get_hdr) (buf);
	if (!mh) {
		return NULL;
	}
	RList *ret = r_list_new ();
	if (!ret) {
		free (mh);
		return NULL;
	}
	ret->free = free;
	ut64 addr = pa2va (bf, 0);
	ut64 paddr = 0;

	r_list_append (ret, r_bin_field_new (addr, addr, 1, "header", "mach0_header", "mach0_header", true));
	addr += 0x20 - 4;
	paddr += 0x20 - 4;
	bool is64 = mh->cputype >> 16;
	if (is64) {
		addr += 4;
		paddr += 4;
	}

	bool isBe = false;
	switch (mh->cputype) {
	case CPU_TYPE_POWERPC:
	case CPU_TYPE_POWERPC64:
		isBe = true;
		break;
	}

	int n;
	char load_command_flagname[128];
	for (n = 0; n < mh->ncmds && paddr < length; n++) {
		ut32 lcType = r_buf_read_ble32_at (buf, paddr, isBe);
		ut32 word = r_buf_read_ble32_at (buf, paddr + 4, isBe);
		if (paddr + 8 > length) {
			break;
		}
		ut32 lcSize = word;
		word &= 0xFFFFFF;
		if (lcSize < 1) {
			eprintf ("Invalid size for a load command\n");
			break;
		}
		if (word == 0) {
			break;
		}
		const char *pf_definition = cmd_to_pf_definition (lcType);
		if (pf_definition) {
			snprintf (load_command_flagname, sizeof (load_command_flagname), "load_command_%d_%s", n, cmd_to_string (lcType));
			r_list_append (ret, r_bin_field_new (addr, addr, 1, load_command_flagname, pf_definition, pf_definition, true));
		}
		switch (lcType) {
		case LC_BUILD_VERSION: {
			ut32 ntools = r_buf_read_le32_at (buf, paddr + 20);
			ut64 off = 24;
			int j = 0;
			char tool_flagname[32];
			while (off < lcSize && ntools--) {
				snprintf (tool_flagname, sizeof (tool_flagname), "tool_%d", j++);
				r_list_append (ret, r_bin_field_new (addr + off, addr + off, 1, tool_flagname, "mach0_build_version_tool", "mach0_build_version_tool", true));
				off += 8;
			}
			break;
		}
		case LC_SEGMENT:
		case LC_SEGMENT_64: {
			ut32 nsects = r_buf_read_le32_at (buf, addr + (is64 ? 64 : 48));
			ut64 off = is64 ? 72 : 56;
			size_t i, j = 0;
			char section_flagname[128];
			for (i = 0; i < nsects && (addr + off) < length && off < lcSize; i++) {
				const char *sname = is64? "mach0_section64": "mach0_section";
				snprintf (section_flagname, sizeof (section_flagname), "section_%u", (ut32)j++);
				RBinField *f = r_bin_field_new (addr + off, addr + off, 1, section_flagname, sname, sname, true);
				r_list_append (ret, f);
				off += is64? 80: 68;
			}
			break;
		default:
			// TODO
			break;
		}
		}
		addr += word;
		paddr += word;
	}
	free (mh);
	return ret;
}