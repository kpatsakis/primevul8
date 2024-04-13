static size_t get_word_size(struct MACH0_(obj_t) *bin) {
	const size_t word_size = MACH0_(get_bits)(bin) / 8;
	return R_MAX (word_size, 4);
}