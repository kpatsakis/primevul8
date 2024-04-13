static void *move_iter(struct timer_list_iter *iter, loff_t offset)
{
	for (; offset; offset--) {
		iter->cpu = cpumask_next(iter->cpu, cpu_online_mask);
		if (iter->cpu >= nr_cpu_ids) {
#ifdef CONFIG_GENERIC_CLOCKEVENTS
			if (!iter->second_pass) {
				iter->cpu = -1;
				iter->second_pass = true;
			} else
				return NULL;
#else
			return NULL;
#endif
		}
	}
	return iter;
}