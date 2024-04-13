int git_checkout_tree(
	git_repository *repo,
	const git_object *treeish,
	const git_checkout_options *opts)
{
	int error;
	git_index *index;
	git_tree *tree = NULL;
	git_iterator *tree_i = NULL;
	git_iterator_options iter_opts = GIT_ITERATOR_OPTIONS_INIT;

	if (!treeish && !repo) {
		git_error_set(GIT_ERROR_CHECKOUT,
			"must provide either repository or tree to checkout");
		return -1;
	}
	if (treeish && repo && git_object_owner(treeish) != repo) {
		git_error_set(GIT_ERROR_CHECKOUT,
			"object to checkout does not match repository");
		return -1;
	}

	if (!repo)
		repo = git_object_owner(treeish);

	if (treeish) {
		if (git_object_peel((git_object **)&tree, treeish, GIT_OBJECT_TREE) < 0) {
			git_error_set(
				GIT_ERROR_CHECKOUT, "provided object cannot be peeled to a tree");
			return -1;
		}
	}
	else {
		if ((error = checkout_lookup_head_tree(&tree, repo)) < 0) {
			if (error != GIT_EUNBORNBRANCH)
				git_error_set(
					GIT_ERROR_CHECKOUT,
					"HEAD could not be peeled to a tree and no treeish given");
			return error;
		}
	}

	if ((error = git_repository_index(&index, repo)) < 0)
		return error;

	if (opts && (opts->checkout_strategy & GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH)) {
		iter_opts.pathlist.count = opts->paths.count;
		iter_opts.pathlist.strings = opts->paths.strings;
	}

	if (!(error = git_iterator_for_tree(&tree_i, tree, &iter_opts)))
		error = git_checkout_iterator(tree_i, index, opts);

	git_iterator_free(tree_i);
	git_index_free(index);
	git_tree_free(tree);

	return error;
}