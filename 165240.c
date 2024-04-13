unshape_window (GSWindow *window)
{
        gdk_window_shape_combine_region (gtk_widget_get_window (GTK_WIDGET (window)),
                                         NULL,
                                         0,
                                         0);
}