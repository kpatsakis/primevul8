gs_window_set_screen (GSWindow  *window,
                      GdkScreen *screen)
{

        g_return_if_fail (GS_IS_WINDOW (window));
        g_return_if_fail (GDK_IS_SCREEN (screen));

        gtk_window_set_screen (GTK_WINDOW (window), screen);
}