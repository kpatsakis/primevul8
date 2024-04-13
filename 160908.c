get_iter_from_proxy(GtkTreeStore *store,
		    GtkTreeIter *iter,
		    GDBusProxy *proxy)
{
	return iter_search(store, iter, NULL, compare_path,
			   (gpointer) g_dbus_proxy_get_object_path (proxy));
}