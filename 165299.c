gs_window_raise (GSWindow *window)
{
        GdkWindow *win;

        g_return_if_fail (GS_IS_WINDOW (window));

        gs_debug ("Raising screensaver window");

        win = gtk_widget_get_window (GTK_WIDGET (window));

        gdk_window_raise (win);
}