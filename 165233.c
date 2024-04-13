on_realized (GtkWidget *widget, GSWindow *window)
{
    gs_window_clear_to_background_surface (window);
    gtk_widget_queue_draw (widget);
}