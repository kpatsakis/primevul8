remove_command_watches (GSWindow *window)
{
        if (window->priv->lock_watch_id != 0) {
                g_source_remove (window->priv->lock_watch_id);
                window->priv->lock_watch_id = 0;
        }
        if (window->priv->keyboard_watch_id != 0) {
                g_source_remove (window->priv->keyboard_watch_id);
                window->priv->keyboard_watch_id = 0;
        }
}