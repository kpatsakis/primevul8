gs_window_real_hide (GtkWidget *widget)
{
        GSWindow *window;

        window = GS_WINDOW (widget);

        gdk_window_remove_filter (NULL, (GdkFilterFunc)xevent_filter, window);

        remove_watchdog_timer (window);

        if (GTK_WIDGET_CLASS (gs_window_parent_class)->hide) {
                GTK_WIDGET_CLASS (gs_window_parent_class)->hide (widget);
        }
}