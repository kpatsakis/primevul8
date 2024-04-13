static inline unsigned calc_index(unsigned expires, unsigned lvl)
{
	expires = (expires + LVL_GRAN(lvl)) >> LVL_SHIFT(lvl);
	return LVL_OFFS(lvl) + (expires & LVL_MASK);
}