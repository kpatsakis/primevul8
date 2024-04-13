_nm_singleton_instance_weak_cb(gpointer data, GObject *where_the_object_was)
{
    nm_assert(g_slist_find(_singletons, where_the_object_was));

    _singletons = g_slist_remove(_singletons, where_the_object_was);
}