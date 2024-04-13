maybe_handle_activity (GSWindow *window)
{
        gboolean handled;

        handled = FALSE;

        /* if we already have a socket then don't bother */
        if (! window->priv->lock_socket
            && gtk_widget_get_sensitive (GTK_WIDGET (window))) {
                g_signal_emit (window, signals [ACTIVITY], 0, &handled);
        }

        return handled;
}