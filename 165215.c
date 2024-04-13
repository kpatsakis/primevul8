watchdog_timer (GSWindow *window)
{
        GtkWidget *widget = GTK_WIDGET (window);

        gdk_window_focus (gtk_widget_get_window (widget), GDK_CURRENT_TIME);

        return TRUE;
}