static ut64 pa2va(RBinFile *bf, ut64 offset) {
	r_return_val_if_fail (bf && bf->rbin, offset);
	RIO *io = bf->rbin->iob.io;
	if (!io || !io->va) {
		return offset;
	}
	struct MACH0_(obj_t) *bin = bf->o->bin_obj;
	return bin? offset_to_vaddr (bin, offset): offset;
}