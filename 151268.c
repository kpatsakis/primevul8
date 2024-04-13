int get_activity_position(struct activity *act[], unsigned int act_flag, int stop)
{
	int i;

	for (i = 0; i < NR_ACT; i++) {
		if (act[i]->id == act_flag)
			return i;
	}

	if (stop) {
		PANIC((int) act_flag);
	}

	return -1;
}