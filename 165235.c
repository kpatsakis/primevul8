gs_window_real_button_press_event (GtkWidget      *widget,
                                   GdkEventButton *event)
{
        GSWindow *window;

        window = GS_WINDOW (widget);
        maybe_handle_activity (window);

        return FALSE;
}