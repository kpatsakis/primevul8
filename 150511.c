static int walk_exports(struct MACH0_(obj_t) *bin, RExportsIterator iterator, void *ctx) {
	RList *states = NULL;
	r_return_val_if_fail (bin, 0);
	if (!bin->dyld_info) {
		return 0;
	}

	size_t count = 0;
	ut8 *p = NULL;
	ut64 size = bin->dyld_info->export_size;
	if (!size || size >= SIZE_MAX) {
		return 0;
	}
	ut8 *trie = calloc (size + 1, 1);
	if (!trie) {
		return 0;
	}
	ut8 *end = trie + size;
	if (r_buf_read_at (bin->b, bin->dyld_info->export_off, trie, bin->dyld_info->export_size) != size) {
		goto beach;
	}

	states = r_list_newf ((RListFree)free);
	if (!states) {
		goto beach;
	}

	RTrieState *root = R_NEW0 (RTrieState);
	if (!root) {
		goto beach;
	}
	root->node = trie;
	root->i = 0;
	root->label = NULL;
	r_list_push (states, root);

	do {
		RTrieState * state = r_list_get_top (states);
		p = state->node;
		ut64 len = read_uleb128 (&p, end);
		if (len == UT64_MAX) {
			break;
		}
		if (len) {
			ut64 flags = read_uleb128 (&p, end);
			if (flags == UT64_MAX) {
				break;
			}
			ut64 offset = read_uleb128 (&p, end);
			if (offset == UT64_MAX) {
				break;
			}
			ut64 resolver = 0;
			bool isReexport = flags & EXPORT_SYMBOL_FLAGS_REEXPORT;
			bool hasResolver = flags & EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER;
			if (hasResolver) {
				ut64 res = read_uleb128 (&p, end);
				if (res == UT64_MAX) {
					break;
				}
				resolver = res + bin->header_at;
			} else if (isReexport) {
				p += strlen ((char*) p) + 1;
				// TODO: handle this
			}
			if (!isReexport) {
				offset += bin->header_at;
			}
			if (iterator && !isReexport) {
				char * name = NULL;
				RListIter *iter;
				RTrieState *s;
				r_list_foreach (states, iter, s) {
					if (!s->label) {
						continue;
					}
					name = r_str_append (name, s->label);
				}
				if (!name) {
					bprintf ("malformed export trie %d\n", __LINE__);
					goto beach;
				}
				if (hasResolver) {
					char * stub_name = r_str_newf ("stub.%s", name);
					iterator (bin, stub_name, flags, offset, ctx);
					iterator (bin, name, flags, resolver, ctx);
					R_FREE (stub_name);
				} else {
					iterator (bin, name, flags, offset, ctx);
				}
				R_FREE (name);
			}
			if (!isReexport) {
				if (hasResolver) {
					count++;
				}
				count++;
			}
		}
		ut64 child_count = read_uleb128 (&p, end);
		if (child_count == UT64_MAX) {
			goto beach;
		}
		if (state->i == child_count) {
			free (r_list_pop (states));
			continue;
		}
		if (!state->next_child) {
			state->next_child = p;
		} else {
			p = state->next_child;
		}
		RTrieState * next = R_NEW0 (RTrieState);
		if (!next) {
			goto beach;
		}
		next->label = (char *) p;
		p += strlen (next->label) + 1;
		if (p >= end) {
			bprintf ("malformed export trie %d\n", __LINE__);
			R_FREE (next);
			goto beach;
		}
		ut64 tr = read_uleb128 (&p, end);
		if (tr == UT64_MAX || tr >= size) {
			R_FREE (next);
			goto beach;
		}
		next->node = trie + (size_t)tr;
		if (next->node >= end) {
			bprintf ("malformed export trie %d\n", __LINE__);
			R_FREE (next);
			goto beach;
		}
		{
			// avoid loops
			RListIter *it;
			RTrieState *s;
			r_list_foreach (states, it, s) {
				if (s->node == next->node) {
					bprintf ("malformed export trie %d\n", __LINE__);
					R_FREE (next);
					goto beach;
				}
			}
		}
		next->i = 0;
		state->i++;
		state->next_child = p;
		r_list_push (states, next);
	} while (r_list_length (states));

beach:
	r_list_free (states);
	R_FREE (trie);
	return count;
}