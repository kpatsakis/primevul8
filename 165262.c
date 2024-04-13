get_outside_region (GSWindow *window)
{
        int             i;
        cairo_region_t *region;

        region = cairo_region_create ();
        for (i = 0; i < window->priv->monitor; i++) {
                GdkRectangle geometry;
                cairo_rectangle_int_t rectangle;

                gdk_screen_get_monitor_geometry (gtk_window_get_screen (GTK_WINDOW (window)),
                                                   i, &geometry);
                rectangle.x = geometry.x;
                rectangle.y = geometry.y;
                rectangle.width = geometry.width;
                rectangle.height = geometry.height;
                cairo_region_union_rectangle (region, &rectangle);
        }

        return region;
}