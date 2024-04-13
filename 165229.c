lock_socket_destroyed (GtkWidget *widget,
                       GSWindow  *window)
{
        g_signal_handlers_disconnect_by_func (widget, lock_socket_show, window);
        g_signal_handlers_disconnect_by_func (widget, lock_socket_destroyed, window);
        g_signal_handlers_disconnect_by_func (widget, lock_plug_added, window);
        g_signal_handlers_disconnect_by_func (widget, lock_plug_removed, window);

        window->priv->lock_socket = NULL;
}