gs_window_get_gdk_window (GSWindow *window)
{
        g_return_val_if_fail (GS_IS_WINDOW (window), NULL);

        return gtk_widget_get_window (GTK_WIDGET (window));
}