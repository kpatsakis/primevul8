static void free_watch_adapter(struct watch_adapter *watch)
{
	kfree(watch->watch.node);
	kfree(watch->token);
	kfree(watch);
}