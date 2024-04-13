gs_window_destroy (GSWindow *window)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        gs_window_cancel_unlock_request (window);

        gtk_widget_destroy (GTK_WIDGET (window));
}