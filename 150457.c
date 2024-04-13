static void fill_exports_list(struct MACH0_(obj_t) *bin, const char *name, ut64 flags, ut64 offset, void *ctx) {
	RList *list = (RList*) ctx;
	RBinSymbol *sym = R_NEW0 (RBinSymbol);
	if (!sym) {
		return;
	}
	sym->vaddr = offset_to_vaddr (bin, offset);
	sym->paddr = offset;
	sym->type = "EXT";
	sym->name = strdup (name);
	sym->bind = R_BIN_BIND_GLOBAL_STR;
	r_list_append (list, sym);
}