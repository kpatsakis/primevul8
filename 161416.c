static void hrtimer_switch_to_hres(void)
{
	struct hrtimer_cpu_base *base = this_cpu_ptr(&hrtimer_bases);

	if (tick_init_highres()) {
		printk(KERN_WARNING "Could not switch to high resolution "
				    "mode on CPU %d\n", base->cpu);
		return;
	}
	base->hres_active = 1;
	hrtimer_resolution = HIGH_RES_NSEC;

	tick_setup_sched_timer();
	/* "Retrigger" the interrupt to get things going */
	retrigger_next_event(NULL);
}