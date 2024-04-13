static bool parse_dysymtab(struct MACH0_(obj_t) *bin, ut64 off) {
	size_t len, i;
	ut32 size_tab;
	ut8 dysym[sizeof (struct dysymtab_command)] = {0};
	ut8 dytoc[sizeof (struct dylib_table_of_contents)] = {0};
	ut8 dymod[sizeof (struct MACH0_(dylib_module))] = {0};
	ut8 idsyms[sizeof (ut32)] = {0};

	if (off > bin->size || off + sizeof (struct dysymtab_command) > bin->size) {
		return false;
	}

	len = r_buf_read_at (bin->b, off, dysym, sizeof (struct dysymtab_command));
	if (len != sizeof (struct dysymtab_command)) {
		bprintf ("Error: read (dysymtab)\n");
		return false;
	}

	bin->dysymtab.cmd = r_read_ble32 (&dysym[0], bin->big_endian);
	bin->dysymtab.cmdsize = r_read_ble32 (&dysym[4], bin->big_endian);
	bin->dysymtab.ilocalsym = r_read_ble32 (&dysym[8], bin->big_endian);
	bin->dysymtab.nlocalsym = r_read_ble32 (&dysym[12], bin->big_endian);
	bin->dysymtab.iextdefsym = r_read_ble32 (&dysym[16], bin->big_endian);
	bin->dysymtab.nextdefsym = r_read_ble32 (&dysym[20], bin->big_endian);
	bin->dysymtab.iundefsym = r_read_ble32 (&dysym[24], bin->big_endian);
	bin->dysymtab.nundefsym = r_read_ble32 (&dysym[28], bin->big_endian);
	bin->dysymtab.tocoff = r_read_ble32 (&dysym[32], bin->big_endian);
	bin->dysymtab.ntoc = r_read_ble32 (&dysym[36], bin->big_endian);
	bin->dysymtab.modtaboff = r_read_ble32 (&dysym[40], bin->big_endian);
	bin->dysymtab.nmodtab = r_read_ble32 (&dysym[44], bin->big_endian);
	bin->dysymtab.extrefsymoff = r_read_ble32 (&dysym[48], bin->big_endian);
	bin->dysymtab.nextrefsyms = r_read_ble32 (&dysym[52], bin->big_endian);
	bin->dysymtab.indirectsymoff = r_read_ble32 (&dysym[56], bin->big_endian);
	bin->dysymtab.nindirectsyms = r_read_ble32 (&dysym[60], bin->big_endian);
	bin->dysymtab.extreloff = r_read_ble32 (&dysym[64], bin->big_endian);
	bin->dysymtab.nextrel = r_read_ble32 (&dysym[68], bin->big_endian);
	bin->dysymtab.locreloff = r_read_ble32 (&dysym[72], bin->big_endian);
	bin->dysymtab.nlocrel = r_read_ble32 (&dysym[76], bin->big_endian);

	bin->ntoc = bin->dysymtab.ntoc;
	if (bin->ntoc > 0) {
		if (!(bin->toc = calloc (bin->ntoc, sizeof (struct dylib_table_of_contents)))) {
			r_sys_perror ("calloc (toc)");
			return false;
		}
		if (!UT32_MUL (&size_tab, bin->ntoc, sizeof (struct dylib_table_of_contents))){
			R_FREE (bin->toc);
			return false;
		}
		if (!size_tab){
			R_FREE (bin->toc);
			return false;
		}
		if (bin->dysymtab.tocoff > bin->size || bin->dysymtab.tocoff + size_tab > bin->size){
			R_FREE (bin->toc);
			return false;
		}
		for (i = 0; i < bin->ntoc; i++) {
			len = r_buf_read_at (bin->b, bin->dysymtab.tocoff +
				i * sizeof (struct dylib_table_of_contents),
				dytoc, sizeof (struct dylib_table_of_contents));
			if (len != sizeof (struct dylib_table_of_contents)) {
				bprintf ("Error: read (toc)\n");
				R_FREE (bin->toc);
				return false;
			}
			bin->toc[i].symbol_index = r_read_ble32 (&dytoc[0], bin->big_endian);
			bin->toc[i].module_index = r_read_ble32 (&dytoc[4], bin->big_endian);
		}
	}
	bin->nmodtab = bin->dysymtab.nmodtab;
	ut64 max_nmodtab = (bin->size - bin->dysymtab.modtaboff) / sizeof (struct MACH0_(dylib_module));
	if (bin->nmodtab > 0 && bin->nmodtab <= max_nmodtab) {
		if (!(bin->modtab = calloc (bin->nmodtab, sizeof (struct MACH0_(dylib_module))))) {
			r_sys_perror ("calloc (modtab)");
			return false;
		}
		if (!UT32_MUL (&size_tab, bin->nmodtab, sizeof (struct MACH0_(dylib_module)))){
			R_FREE (bin->modtab);
			return false;
		}
		if (!size_tab){
			R_FREE (bin->modtab);
			return false;
		}
		if (bin->dysymtab.modtaboff > bin->size || \
		  bin->dysymtab.modtaboff + size_tab > bin->size){
			R_FREE (bin->modtab);
			return false;
		}
		for (i = 0; i < bin->nmodtab; i++) {
			len = r_buf_read_at (bin->b, bin->dysymtab.modtaboff +
				i * sizeof (struct MACH0_(dylib_module)),
				dymod, sizeof (struct MACH0_(dylib_module)));
			if (len == -1) {
				bprintf ("Error: read (modtab)\n");
				R_FREE (bin->modtab);
				return false;
			}

			bin->modtab[i].module_name = r_read_ble32 (&dymod[0], bin->big_endian);
			bin->modtab[i].iextdefsym = r_read_ble32 (&dymod[4], bin->big_endian);
			bin->modtab[i].nextdefsym = r_read_ble32 (&dymod[8], bin->big_endian);
			bin->modtab[i].irefsym = r_read_ble32 (&dymod[12], bin->big_endian);
			bin->modtab[i].nrefsym = r_read_ble32 (&dymod[16], bin->big_endian);
			bin->modtab[i].ilocalsym = r_read_ble32 (&dymod[20], bin->big_endian);
			bin->modtab[i].nlocalsym = r_read_ble32 (&dymod[24], bin->big_endian);
			bin->modtab[i].iextrel = r_read_ble32 (&dymod[28], bin->big_endian);
			bin->modtab[i].nextrel = r_read_ble32 (&dymod[32], bin->big_endian);
			bin->modtab[i].iinit_iterm = r_read_ble32 (&dymod[36], bin->big_endian);
			bin->modtab[i].ninit_nterm = r_read_ble32 (&dymod[40], bin->big_endian);
#if R_BIN_MACH064
			bin->modtab[i].objc_module_info_size = r_read_ble32 (&dymod[44], bin->big_endian);
			bin->modtab[i].objc_module_info_addr = r_read_ble64 (&dymod[48], bin->big_endian);
#else
			bin->modtab[i].objc_module_info_addr = r_read_ble32 (&dymod[44], bin->big_endian);
			bin->modtab[i].objc_module_info_size = r_read_ble32 (&dymod[48], bin->big_endian);
#endif
		}
	}
	bin->nindirectsyms = bin->dysymtab.nindirectsyms;
	if (bin->nindirectsyms > 0) {
		if (!(bin->indirectsyms = calloc (bin->nindirectsyms, sizeof (ut32)))) {
			r_sys_perror ("calloc (indirectsyms)");
			return false;
		}
		if (!UT32_MUL (&size_tab, bin->nindirectsyms, sizeof (ut32))){
			R_FREE (bin->indirectsyms);
			return false;
		}
		if (!size_tab){
			R_FREE (bin->indirectsyms);
			return false;
		}
		if (bin->dysymtab.indirectsymoff > bin->size || \
				bin->dysymtab.indirectsymoff + size_tab > bin->size){
			R_FREE (bin->indirectsyms);
			return false;
		}
		for (i = 0; i < bin->nindirectsyms; i++) {
			len = r_buf_read_at (bin->b, bin->dysymtab.indirectsymoff + i * sizeof (ut32), idsyms, 4);
			if (len == -1) {
				bprintf ("Error: read (indirect syms)\n");
				R_FREE (bin->indirectsyms);
				return false;
			}
			bin->indirectsyms[i] = r_read_ble32 (&idsyms[0], bin->big_endian);
		}
	}
	/* TODO extrefsyms, extrel, locrel */
	return true;
}