keyboard_socket_destroyed (GtkWidget *widget,
                           GSWindow  *window)
{
        g_signal_handlers_disconnect_by_func (widget, keyboard_socket_destroyed, window);
        g_signal_handlers_disconnect_by_func (widget, keyboard_plug_added, window);
        g_signal_handlers_disconnect_by_func (widget, keyboard_plug_removed, window);

        window->priv->keyboard_socket = NULL;
}