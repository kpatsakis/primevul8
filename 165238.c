clear_widget (GtkWidget *widget)
{
        GdkRGBA rgba = { 0.0, 0.0, 0.0, 1.0 };

        if (!gtk_widget_get_realized (widget))
                return;

        gtk_widget_override_background_color (widget, GTK_STATE_FLAG_NORMAL, &rgba);
        gtk_widget_queue_draw (GTK_WIDGET (widget));
}