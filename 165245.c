gs_window_real_show (GtkWidget *widget)
{
        GSWindow *window;

        if (GTK_WIDGET_CLASS (gs_window_parent_class)->show) {
                GTK_WIDGET_CLASS (gs_window_parent_class)->show (widget);
        }

        set_invisible_cursor (gtk_widget_get_window (widget), TRUE);

        window = GS_WINDOW (widget);
        if (window->priv->timer) {
                g_timer_destroy (window->priv->timer);
        }
        window->priv->timer = g_timer_new ();

        remove_watchdog_timer (window);
        add_watchdog_timer (window, 30);

        select_popup_events ();
        window_select_shape_events (window);
        gdk_window_add_filter (NULL, (GdkFilterFunc)xevent_filter, window);
}