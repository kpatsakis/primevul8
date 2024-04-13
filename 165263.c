remove_key_events (GSWindow *window)
{
        window->priv->key_events = g_list_reverse (window->priv->key_events);

        while (window->priv->key_events) {
                GdkEventKey *event = window->priv->key_events->data;

                gdk_event_free ((GdkEvent *)event);
                window->priv->key_events = g_list_delete_link (window->priv->key_events,
                                                               window->priv->key_events);
        }
}