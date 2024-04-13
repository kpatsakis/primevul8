gs_window_request_unlock (GSWindow *window)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        gs_debug ("Requesting unlock");

        if (! gtk_widget_get_visible (GTK_WIDGET (window))) {
                gs_debug ("Request unlock but window is not visible!");
                return;
        }

        if (window->priv->lock_watch_id > 0) {
                return;
        }

        if (! window->priv->lock_enabled) {
                add_emit_deactivated_idle (window);

                return;
        }

        if (window->priv->popup_dialog_idle_id == 0) {
                add_popup_dialog_idle (window);
        }

        window_set_dialog_up (window, TRUE);
}