static bool walk_bind_chains_callback(void * context, RFixupEventDetails * event_details) {
	r_return_val_if_fail (event_details->type == R_FIXUP_EVENT_BIND || event_details->type == R_FIXUP_EVENT_BIND_AUTH, false);
	RWalkBindChainsContext *ctx = context;
	ut8 *imports = ctx->imports;
	struct MACH0_(obj_t) *bin = event_details->bin;
	ut32 imports_count = bin->fixups_header.imports_count;
	ut32 fixups_offset = bin->fixups_offset;
	ut32 fixups_size = bin->fixups_size;
	ut32 imports_format = bin->fixups_header.imports_format;
	ut32 import_index = ((RFixupBindEventDetails *) event_details)->ordinal;
	ut64 addend = 0;
	if (event_details->type != R_FIXUP_EVENT_BIND_AUTH) {
		addend = ((RFixupBindEventDetails *) event_details)->addend;
	}

	if (import_index < imports_count) {
		ut64 name_offset;
		switch (imports_format) {
			case DYLD_CHAINED_IMPORT: {
				struct dyld_chained_import * item = &((struct dyld_chained_import *) imports)[import_index];
				name_offset = item->name_offset;
				break;
			}
			case DYLD_CHAINED_IMPORT_ADDEND: {
				struct dyld_chained_import_addend * item = &((struct dyld_chained_import_addend *) imports)[import_index];
				name_offset = item->name_offset;
				addend += item->addend;
				break;
			}
			case DYLD_CHAINED_IMPORT_ADDEND64: {
				struct dyld_chained_import_addend64 * item = &((struct dyld_chained_import_addend64 *) imports)[import_index];
				name_offset = item->name_offset;
				addend += item->addend;
				break;
			}
			default:
				bprintf ("Unsupported imports format\n");
				return false;
		}

		ut64 symbols_offset = bin->fixups_header.symbols_offset + fixups_offset;

		if (symbols_offset + name_offset + 1 < fixups_offset + fixups_size) {
			char *name = r_buf_get_string (bin->b, symbols_offset + name_offset);
			if (name) {
				struct reloc_t *reloc = R_NEW0 (struct reloc_t);
				if (!reloc) {
					free (name);
					return false;
				}
				reloc->addr = offset_to_vaddr (bin, event_details->offset);
				reloc->offset = event_details->offset;
				reloc->ord = import_index;
				reloc->type = R_BIN_RELOC_64;
				reloc->size = 8;
				reloc->addend = addend;
				r_str_ncpy (reloc->name, name, sizeof (reloc->name) - 1);
				r_skiplist_insert_autofree (ctx->relocs, reloc);
				free (name);
			} else if (bin->verbose) {
				eprintf ("Malformed chained bind: failed to read name\n");
			}
		} else if (bin->verbose) {
			eprintf ("Malformed chained bind: name_offset out of bounds\n");
		}
	} else if (bin->verbose) {
		eprintf ("Malformed chained bind: import out of length\n");
	}

	return true;
}