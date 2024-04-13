_nm_singleton_instance_register_destruction(GObject *instance)
{
    g_return_if_fail(G_IS_OBJECT(instance));

    /* Don't allow registration after shutdown. We only destroy the singletons
     * once. */
    g_return_if_fail(!_singletons_shutdown);

    g_object_weak_ref(instance, _nm_singleton_instance_weak_cb, NULL);

    _singletons = g_slist_prepend(_singletons, instance);
}