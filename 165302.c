gs_window_set_lock_enabled (GSWindow *window,
                            gboolean  lock_enabled)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        if (window->priv->lock_enabled == lock_enabled) {
                return;
        }

        window->priv->lock_enabled = lock_enabled;
        g_object_notify (G_OBJECT (window), "lock-enabled");
}