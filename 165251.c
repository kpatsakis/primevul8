gs_window_get_screen (GSWindow  *window)
{
        g_return_val_if_fail (GS_IS_WINDOW (window), NULL);

        return gtk_window_get_screen (GTK_WINDOW (window));
}