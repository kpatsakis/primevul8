lock_socket_show (GtkWidget *widget,
                  GSWindow  *window)
{
        gtk_widget_child_focus (window->priv->lock_socket, GTK_DIR_TAB_FORWARD);

        /* send queued events to the dialog */
        forward_key_events (window);
}