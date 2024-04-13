gs_window_set_monitor (GSWindow *window,
                       int       monitor)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        if (window->priv->monitor == monitor) {
                return;
        }

        window->priv->monitor = monitor;

        gtk_widget_queue_resize (GTK_WIDGET (window));

        g_object_notify (G_OBJECT (window), "monitor");
}