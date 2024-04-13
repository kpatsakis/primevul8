gs_window_real_realize (GtkWidget *widget)
{
        if (GTK_WIDGET_CLASS (gs_window_parent_class)->realize) {
                GTK_WIDGET_CLASS (gs_window_parent_class)->realize (widget);
        }

        gs_window_override_user_time (GS_WINDOW (widget));

        gs_window_move_resize_window (GS_WINDOW (widget), TRUE, TRUE);

        g_signal_connect (gtk_window_get_screen (GTK_WINDOW (widget)),
                          "size_changed",
                          G_CALLBACK (screen_size_changed),
                          widget);
}