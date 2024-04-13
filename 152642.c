int parse_sa_P_opt(char *argv[], int *opt, unsigned int *flags, struct activity *act[])
{
	int p;

	p = get_activity_position(act, A_CPU, EXIT_IF_NOT_FOUND);

	if (argv[++(*opt)]) {
		if (parse_values(argv[*opt], act[p]->bitmap->b_array,
			     act[p]->bitmap->b_size, K_LOWERALL))
			return 1;
		(*opt)++;
		return 0;
	}

	return 1;
}