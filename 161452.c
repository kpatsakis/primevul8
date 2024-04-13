static int __collect_expired_timers(struct timer_base *base,
				    struct hlist_head *heads)
{
	unsigned long clk = base->clk;
	struct hlist_head *vec;
	int i, levels = 0;
	unsigned int idx;

	for (i = 0; i < LVL_DEPTH; i++) {
		idx = (clk & LVL_MASK) + i * LVL_SIZE;

		if (__test_and_clear_bit(idx, base->pending_map)) {
			vec = base->vectors + idx;
			hlist_move_list(vec, heads++);
			levels++;
		}
		/* Is it time to look at the next level? */
		if (clk & LVL_CLK_MASK)
			break;
		/* Shift clock for the next level granularity */
		clk >>= LVL_CLK_SHIFT;
	}
	return levels;
}