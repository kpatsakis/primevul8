static struct watch_adapter *alloc_watch_adapter(const char *path,
						 const char *token)
{
	struct watch_adapter *watch;

	watch = kzalloc(sizeof(*watch), GFP_KERNEL);
	if (watch == NULL)
		goto out_fail;

	watch->watch.node = kstrdup(path, GFP_KERNEL);
	if (watch->watch.node == NULL)
		goto out_free;

	watch->token = kstrdup(token, GFP_KERNEL);
	if (watch->token == NULL)
		goto out_free;

	return watch;

out_free:
	free_watch_adapter(watch);

out_fail:
	return NULL;
}