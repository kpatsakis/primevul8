int get_activity_nr(struct activity *act[], unsigned int option, int count_outputs)
{
	int i, n = 0;
	unsigned int msk;

	for (i = 0; i < NR_ACT; i++) {
		if ((act[i]->options & option) == option) {

			if (HAS_MULTIPLE_OUTPUTS(act[i]->options) && count_outputs) {
				for (msk = 1; msk < 0x100; msk <<= 1) {
					if ((act[i]->opt_flags & 0xff) & msk) {
						n++;
					}
				}
			}
			else {
				n++;
			}
		}
	}

	return n;
}