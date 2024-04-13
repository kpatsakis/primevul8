static void walk_bind_chains(struct MACH0_(obj_t) *bin, RSkipList *relocs) {
	r_return_if_fail (bin && bin->fixups_offset);

	ut8 *imports = NULL;

	ut32 imports_count = bin->fixups_header.imports_count;
	ut32 fixups_offset = bin->fixups_offset;
	ut32 imports_offset = bin->fixups_header.imports_offset;
	if (!imports_count || !imports_offset) {
		return;
	}
	if (bin->fixups_header.symbols_format != 0) {
		eprintf ("Compressed fixups symbols not supported yet, please file a bug with a sample attached.\n");
		return;
	}

	ut32 imports_format = bin->fixups_header.imports_format;
	ut64 imports_size;
	switch (imports_format) {
		case DYLD_CHAINED_IMPORT:
			imports_size = sizeof (struct dyld_chained_import) * imports_count;
			break;
		case DYLD_CHAINED_IMPORT_ADDEND:
			imports_size = sizeof (struct dyld_chained_import_addend) * imports_count;
			break;
		case DYLD_CHAINED_IMPORT_ADDEND64:
			imports_size = sizeof (struct dyld_chained_import_addend64) * imports_count;
			break;
		default:
			eprintf ("Unsupported chained imports format: %d\n", imports_format);
			goto beach;
	}

	imports = malloc (imports_size);
	if (!imports) {
		goto beach;
	}

	switch (imports_format) {
		case DYLD_CHAINED_IMPORT:
			if (r_buf_fread_at (bin->b, fixups_offset + imports_offset,
					imports, "i", imports_count) != imports_size) {
				goto beach;
			}
			break;
		case DYLD_CHAINED_IMPORT_ADDEND:
			if (r_buf_fread_at (bin->b, fixups_offset + imports_offset,
					imports, "ii", imports_count) != imports_size) {
				goto beach;
			}
			break;
		case DYLD_CHAINED_IMPORT_ADDEND64:
			if (r_buf_fread_at (bin->b, fixups_offset + imports_offset,
					imports, "il", imports_count) != imports_size) {
				goto beach;
			}
			break;
	}

	RWalkBindChainsContext ctx;
	ctx.imports = imports;
	ctx.relocs = relocs;

	MACH0_(iterate_chained_fixups) (bin, 0, UT64_MAX, R_FIXUP_EVENT_MASK_BIND_ALL, &walk_bind_chains_callback, &ctx);

beach:
	free (imports);
}