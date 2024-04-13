void select_default_activity(struct activity *act[])
{
	int p;

	p = get_activity_position(act, A_CPU, EXIT_IF_NOT_FOUND);

	/* Default is CPU activity... */
	if (!get_activity_nr(act, AO_SELECTED, COUNT_ACTIVITIES)) {
		/*
		 * Yet A_CPU activity may not be available in file
		 * since the user can choose not to collect it.
		 */
		act[p]->options |= AO_SELECTED;
	}

	/*
	 * If no CPU's have been selected then select CPU "all".
	 * cpu_bitmap bitmap may be used by several activities (A_CPU, A_PWR_CPU...)
	 */
	if (!count_bits(cpu_bitmap.b_array, BITMAP_SIZE(cpu_bitmap.b_size))) {
		cpu_bitmap.b_array[0] |= 0x01;
	}
}