RSkipList *MACH0_(get_relocs)(struct MACH0_(obj_t) *bin) {
	RSkipList *relocs = NULL;
	RPVector *threaded_binds = NULL;
	size_t wordsize = get_word_size (bin);
	if (bin->dyld_info) {
		ut8 *opcodes, rel_type = 0;
		size_t bind_size, lazy_size, weak_size;

#define CASE(T) case ((T) / 8): rel_type = R_BIN_RELOC_ ## T; break
		switch (wordsize) {
		CASE(8);
		CASE(16);
		CASE(32);
		CASE(64);
		default: return NULL;
		}
#undef CASE
		bind_size = bin->dyld_info->bind_size;
		lazy_size = bin->dyld_info->lazy_bind_size;
		weak_size = bin->dyld_info->weak_bind_size;

		if (!bind_size && !lazy_size) {
			return NULL;
		}

		if ((bind_size + lazy_size)<1) {
			return NULL;
		}
		if (bin->dyld_info->bind_off > bin->size || bin->dyld_info->bind_off + bind_size > bin->size) {
			return NULL;
		}
		if (bin->dyld_info->lazy_bind_off > bin->size || \
			bin->dyld_info->lazy_bind_off + lazy_size > bin->size) {
			return NULL;
		}
		if (bin->dyld_info->bind_off + bind_size + lazy_size > bin->size) {
			return NULL;
		}
		if (bin->dyld_info->weak_bind_off + weak_size > bin->size) {
			return NULL;
		}
		ut64 amount = bind_size + lazy_size + weak_size;
		if (amount == 0 || amount > UT32_MAX) {
			return NULL;
		}
		if (!bin->segs) {
			return NULL;
		}
		relocs = r_skiplist_new ((RListFree) &free, (RListComparator) &reloc_comparator);
		if (!relocs) {
			return NULL;
		}
		opcodes = calloc (1, amount + 1);
		if (!opcodes) {
			r_skiplist_free (relocs);
			return NULL;
		}

		int len = r_buf_read_at (bin->b, bin->dyld_info->bind_off, opcodes, bind_size);
		len += r_buf_read_at (bin->b, bin->dyld_info->lazy_bind_off, opcodes + bind_size, lazy_size);
		len += r_buf_read_at (bin->b, bin->dyld_info->weak_bind_off, opcodes + bind_size + lazy_size, weak_size);
		if (len < amount) {
			bprintf ("Error: read (dyld_info bind) at 0x%08"PFMT64x"\n", (ut64)(size_t)bin->dyld_info->bind_off);
			R_FREE (opcodes);
			r_skiplist_free (relocs);
			return NULL;
		}

		size_t partition_sizes[] = {bind_size, lazy_size, weak_size};
		size_t pidx;
		int opcodes_offset = 0;
		for (pidx = 0; pidx < R_ARRAY_SIZE (partition_sizes); pidx++) {
			size_t partition_size = partition_sizes[pidx];

			ut8 type = 0;
			int lib_ord = 0, seg_idx = -1, sym_ord = -1;
			char *sym_name = NULL;
			size_t j, count, skip;
			st64 addend = 0;
			ut64 addr = bin->segs[0].vmaddr;
			ut64 segment_size = bin->segs[0].filesize;
			if (bin->segs[0].filesize != bin->segs[0].vmsize) {
				// is probably invalid and we should warn the user
			}
			if (segment_size > bin->size) {
				// is probably invalid and we should warn the user
				segment_size = bin->size;
			}
			ut64 segment_end_addr = addr + segment_size;

			ut8 *p = opcodes + opcodes_offset;
			ut8 *end = p + partition_size;
			bool done = false;
			while (!done && p < end) {
				ut8 imm = *p & BIND_IMMEDIATE_MASK;
				ut8 op = *p & BIND_OPCODE_MASK;
				p++;
				switch (op) {
				case BIND_OPCODE_DONE: {
					bool in_lazy_binds = pidx == 1;
					if (!in_lazy_binds) {
						done = true;
					}
					break;
				}
				case BIND_OPCODE_THREADED: {
					switch (imm) {
					case BIND_SUBOPCODE_THREADED_SET_BIND_ORDINAL_TABLE_SIZE_ULEB: {
						ut64 table_size = read_uleb128 (&p, end);
						if (!is_valid_ordinal_table_size (table_size)) {
							bprintf ("Error: BIND_SUBOPCODE_THREADED_SET_BIND_ORDINAL_TABLE_SIZE_ULEB size is wrong\n");
							break;
						}
						if (threaded_binds) {
							r_pvector_free (threaded_binds);
						}
						threaded_binds = r_pvector_new_with_len ((RPVectorFree) &free, table_size);
						if (threaded_binds) {
							sym_ord = 0;
						}
						break;
					}
					case BIND_SUBOPCODE_THREADED_APPLY:
						if (threaded_binds) {
							int cur_seg_idx = (seg_idx != -1)? seg_idx: 0;
							size_t n_threaded_binds = r_pvector_len (threaded_binds);
							while (addr < segment_end_addr) {
								ut8 tmp[8];
								ut64 paddr = addr - bin->segs[cur_seg_idx].vmaddr + bin->segs[cur_seg_idx].fileoff;
								bin->rebasing_buffer = true;
								if (r_buf_read_at (bin->b, paddr, tmp, 8) != 8) {
									break;
								}
								bin->rebasing_buffer = false;
								ut64 raw_ptr = r_read_le64 (tmp);
								bool is_auth = (raw_ptr & (1ULL << 63)) != 0;
								bool is_bind = (raw_ptr & (1ULL << 62)) != 0;
								int ordinal = -1;
								int addend = -1;
								ut64 delta;
								if (is_auth && is_bind) {
									struct dyld_chained_ptr_arm64e_auth_bind *p =
											(struct dyld_chained_ptr_arm64e_auth_bind *) &raw_ptr;
									delta = p->next;
									ordinal = p->ordinal;
								} else if (!is_auth && is_bind) {
									struct dyld_chained_ptr_arm64e_bind *p =
											(struct dyld_chained_ptr_arm64e_bind *) &raw_ptr;
									delta = p->next;
									ordinal = p->ordinal;
									addend = p->addend;
								} else if (is_auth && !is_bind) {
									struct dyld_chained_ptr_arm64e_auth_rebase *p =
											(struct dyld_chained_ptr_arm64e_auth_rebase *) &raw_ptr;
									delta = p->next;
								} else {
									struct dyld_chained_ptr_arm64e_rebase *p =
											(struct dyld_chained_ptr_arm64e_rebase *) &raw_ptr;
									delta = p->next;
								}
								if (ordinal != -1) {
									if (ordinal >= n_threaded_binds) {
										bprintf ("Error: Malformed bind chain\n");
										break;
									}
									struct reloc_t *ref = r_pvector_at (threaded_binds, ordinal);
									if (!ref) {
										bprintf ("Error: Inconsistent bind opcodes\n");
										break;
									}
									struct reloc_t *reloc = R_NEW0 (struct reloc_t);
									if (!reloc) {
										break;
									}
									*reloc = *ref;
									reloc->addr = addr;
									reloc->offset = paddr;
									if (addend != -1) {
										reloc->addend = addend;
									}
									r_skiplist_insert (relocs, reloc);
								}
								addr += delta * wordsize;
								if (!delta) {
									break;
								}
							}
						}
						break;
					default:
						bprintf ("Error: Unexpected BIND_OPCODE_THREADED sub-opcode: 0x%x\n", imm);
					}
					break;
				}
				case BIND_OPCODE_SET_DYLIB_ORDINAL_IMM:
					lib_ord = imm;
					break;
				case BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB:
					lib_ord = read_uleb128 (&p, end);
					break;
				case BIND_OPCODE_SET_DYLIB_SPECIAL_IMM:
					lib_ord = imm? (st8)(BIND_OPCODE_MASK | imm) : 0;
					break;
				case BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM: {
					sym_name = (char*)p;
					while (*p++ && p < end) {
						/* empty loop */
					}
					if (threaded_binds) {
						break;
					}
					sym_ord = -1;
					if (bin->symtab && bin->dysymtab.nundefsym < UT16_MAX) {
						for (j = 0; j < bin->dysymtab.nundefsym; j++) {
							size_t stridx = 0;
							bool found = false;
							int iundefsym = bin->dysymtab.iundefsym;
							if (iundefsym >= 0 && iundefsym < bin->nsymtab) {
								int sidx = iundefsym + j;
								if (sidx < 0 || sidx >= bin->nsymtab) {
									continue;
								}
								stridx = bin->symtab[sidx].n_strx;
								if (stridx >= bin->symstrlen) {
									continue;
								}
								found = true;
							}
							if (found && !strcmp ((const char *)bin->symstr + stridx, sym_name)) {
								sym_ord = j;
								break;
							}
						}
					}
					break;
				}
				case BIND_OPCODE_SET_TYPE_IMM:
					type = imm;
					break;
				case BIND_OPCODE_SET_ADDEND_SLEB:
					addend = r_sleb128 ((const ut8 **)&p, end);
					break;
				case BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:
					seg_idx = imm;
					if (seg_idx >= bin->nsegs) {
						bprintf ("Error: BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB"
							" has unexistent segment %d\n", seg_idx);
						free (opcodes);
						r_skiplist_free (relocs);
						r_pvector_free (threaded_binds);
						return NULL; // early exit to avoid future mayhem
					}
					addr = bin->segs[seg_idx].vmaddr + read_uleb128 (&p, end);
					segment_end_addr = bin->segs[seg_idx].vmaddr \
							+ bin->segs[seg_idx].vmsize;
					break;
				case BIND_OPCODE_ADD_ADDR_ULEB:
					addr += read_uleb128 (&p, end);
					break;
#define DO_BIND() do {\
	if (sym_ord < 0 && !sym_name) break;\
	if (!threaded_binds) {\
		if (seg_idx < 0 ) break;\
		if (!addr) break;\
	}\
	struct reloc_t *reloc = R_NEW0 (struct reloc_t);\
	reloc->addr = addr;\
	if (seg_idx >= 0) {\
		reloc->offset = addr - bin->segs[seg_idx].vmaddr + bin->segs[seg_idx].fileoff;\
		if (type == BIND_TYPE_TEXT_PCREL32)\
			reloc->addend = addend - (bin->baddr + addr);\
		else\
			reloc->addend = addend;\
	} else {\
		reloc->addend = addend;\
	}\
	/* library ordinal ??? */ \
	reloc->ord = lib_ord;\
	reloc->ord = sym_ord;\
	reloc->type = rel_type;\
	if (sym_name)\
		r_str_ncpy (reloc->name, sym_name, 256);\
	if (threaded_binds)\
		r_pvector_set (threaded_binds, sym_ord, reloc);\
	else\
		r_skiplist_insert (relocs, reloc);\
} while (0)
				case BIND_OPCODE_DO_BIND:
					if (!threaded_binds && addr >= segment_end_addr) {
						bprintf ("Error: Malformed DO bind opcode 0x%"PFMT64x"\n", addr);
						goto beach;
					}
					DO_BIND ();
					if (!threaded_binds) {
						addr += wordsize;
					} else {
						sym_ord++;
					}
					break;
				case BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB:
					if (addr >= segment_end_addr) {
						bprintf ("Error: Malformed ADDR ULEB bind opcode\n");
						goto beach;
					}
					DO_BIND ();
					addr += read_uleb128 (&p, end) + wordsize;
					break;
				case BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED:
					if (addr >= segment_end_addr) {
						bprintf ("Error: Malformed IMM SCALED bind opcode\n");
						goto beach;
					}
					DO_BIND ();
					addr += (ut64)imm * (ut64)wordsize + wordsize;
					break;
				case BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB:
					count = read_uleb128 (&p, end);
					skip = read_uleb128 (&p, end);
					for (j = 0; j < count; j++) {
						if (addr >= segment_end_addr) {
							bprintf ("Error: Malformed ULEB TIMES bind opcode\n");
							goto beach;
						}
						DO_BIND ();
						addr += skip + wordsize;
					}
					break;
#undef DO_BIND
				default:
					bprintf ("Error: unknown bind opcode 0x%02x in dyld_info\n", *p);
					R_FREE (opcodes);
					r_pvector_free (threaded_binds);
					return relocs;
				}
			}

			opcodes_offset += partition_size;
		}

		R_FREE (opcodes);
		r_pvector_free (threaded_binds);
		threaded_binds = NULL;
	}

	if (bin->symtab && bin->symstr && bin->sects && bin->indirectsyms) {
		int j;
		int amount = bin->dysymtab.nundefsym;
		if (amount < 0) {
			amount = 0;
		}
		if (!relocs) {
			relocs = r_skiplist_new ((RListFree) &free, (RListComparator) &reloc_comparator);
			if (!relocs) {
				goto beach;
			}
		}
		for (j = 0; j < amount; j++) {
			struct reloc_t *reloc = R_NEW0 (struct reloc_t);
			if (!reloc) {
				break;
			}
			if (parse_import_ptr (bin, reloc, bin->dysymtab.iundefsym + j)) {
				reloc->ord = j;
				r_skiplist_insert_autofree (relocs, reloc);
			} else {
				R_FREE (reloc);
			}
		}
	}

	if (bin->symtab && bin->dysymtab.extreloff && bin->dysymtab.nextrel) {
		if (!relocs) {
			relocs = r_skiplist_new ((RListFree) &free, (RListComparator) &reloc_comparator);
			if (!relocs) {
				goto beach;
			}
		}
		parse_relocation_info (bin, relocs, bin->dysymtab.extreloff, bin->dysymtab.nextrel);
	}

	if (!bin->dyld_info && bin->chained_starts && bin->nsegs && bin->fixups_offset) {
		if (!relocs) {
			relocs = r_skiplist_new ((RListFree) &free, (RListComparator) &reloc_comparator);
			if (!relocs) {
				goto beach;
			}
		}
		walk_bind_chains (bin, relocs);
	}
beach:
	r_pvector_free (threaded_binds);
	return relocs;
}