static bool should_remove_existing(checkout_data *data)
{
	int ignorecase;

	if (git_repository__configmap_lookup(&ignorecase, data->repo, GIT_CONFIGMAP_IGNORECASE) < 0) {
		ignorecase = 0;
	}

	return (ignorecase &&
		(data->strategy & GIT_CHECKOUT_DONT_REMOVE_EXISTING) == 0);
}