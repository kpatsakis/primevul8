gs_window_real_unrealize (GtkWidget *widget)
{
        g_signal_handlers_disconnect_by_func (gtk_window_get_screen (GTK_WINDOW (widget)),
                                              screen_size_changed,
                                              widget);

        if (GTK_WIDGET_CLASS (gs_window_parent_class)->unrealize) {
                GTK_WIDGET_CLASS (gs_window_parent_class)->unrealize (widget);
        }
}