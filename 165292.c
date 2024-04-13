screen_size_changed (GdkScreen *screen,
                     GSWindow  *window)
{
        gs_debug ("Got screen size changed signal");
        gtk_widget_queue_resize (GTK_WIDGET (window));
}