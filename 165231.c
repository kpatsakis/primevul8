gs_window_show_message (GSWindow   *window,
                        const char *summary,
                        const char *body,
                        const char *icon)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        set_info_text_and_icon (window,
                                icon,
                                summary,
                                body);
        gtk_widget_show (window->priv->info_bar);

        if (window->priv->info_bar_timer_id > 0) {
                g_source_remove (window->priv->info_bar_timer_id);
                window->priv->info_bar_timer_id = 0;
        }

        window->priv->info_bar_timer_id = g_timeout_add_seconds (INFO_BAR_SECONDS,
                                                                 (GSourceFunc)info_bar_timeout,
                                                                 window);
}