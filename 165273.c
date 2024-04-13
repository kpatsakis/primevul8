gs_window_finalize (GObject *object)
{
        GSWindow *window;

        g_return_if_fail (object != NULL);
        g_return_if_fail (GS_IS_WINDOW (object));

        window = GS_WINDOW (object);

        g_return_if_fail (window->priv != NULL);

        g_free (window->priv->away_message);
        g_free (window->priv->logout_command);
        g_free (window->priv->keyboard_command);

        if (window->priv->clock_tracker) {
                g_object_unref (window->priv->clock_tracker);
        }

        if (window->priv->info_bar_timer_id > 0) {
            g_source_remove (window->priv->info_bar_timer_id);
            window->priv->info_bar_timer_id = 0;
        }

        remove_watchdog_timer (window);
        remove_popup_dialog_idle (window);

        if (window->priv->timer) {
                g_timer_destroy (window->priv->timer);
        }

        remove_key_events (window);

        remove_command_watches (window);

        gs_window_dialog_finish (window);

        if (window->priv->background_surface) {
               cairo_surface_destroy (window->priv->background_surface);
        }

        G_OBJECT_CLASS (gs_window_parent_class)->finalize (object);
}