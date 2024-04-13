forward_key_events (GSWindow *window)
{
        window->priv->key_events = g_list_reverse (window->priv->key_events);

        while (window->priv->key_events != NULL) {
                GdkEventKey *event = window->priv->key_events->data;

                gtk_window_propagate_key_event (GTK_WINDOW (window), event);

                gdk_event_free ((GdkEvent *)event);
                window->priv->key_events = g_list_delete_link (window->priv->key_events,
                                                               window->priv->key_events);
        }
}