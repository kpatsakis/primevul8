static inline ktime_t hrtimer_update_base(struct hrtimer_cpu_base *base)
{
	ktime_t *offs_real = &base->clock_base[HRTIMER_BASE_REALTIME].offset;
	ktime_t *offs_boot = &base->clock_base[HRTIMER_BASE_BOOTTIME].offset;
	ktime_t *offs_tai = &base->clock_base[HRTIMER_BASE_TAI].offset;

	return ktime_get_update_offsets_now(&base->clock_was_set_seq,
					    offs_real, offs_boot, offs_tai);
}