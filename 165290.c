gs_window_real_scroll_event (GtkWidget      *widget,
                             GdkEventScroll *event)
{
        GSWindow *window;

        window = GS_WINDOW (widget);
        maybe_handle_activity (window);

        return FALSE;
}