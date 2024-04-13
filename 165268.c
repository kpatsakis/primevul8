gs_window_reset_background_surface (GSWindow *window)
{
        cairo_pattern_t *pattern;
        pattern = cairo_pattern_create_for_surface (window->priv->background_surface);
        gdk_window_set_background_pattern (gtk_widget_get_window (GTK_WIDGET (window)),
                                           pattern);
        cairo_pattern_destroy (pattern);
        gtk_widget_queue_draw (GTK_WIDGET (window));
}