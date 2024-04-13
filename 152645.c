void reverse_check_act(unsigned int act_nr)
{
	int i, j;

	for (i = 0; i < NR_ACT; i++) {

		if (IS_SELECTED(act[i]->options)) {

			for (j = 0; j < act_nr; j++) {
				if (id_seq[j] == act[i]->id)
					break;
			}
			if (j == act_nr)
				act[i]->options &= ~AO_SELECTED;
		}
	}
}