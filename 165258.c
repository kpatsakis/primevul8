gs_window_show (GSWindow *window)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        gtk_widget_show (GTK_WIDGET (window));
}