update_geometry (GSWindow *window)
{
        GdkRectangle    geometry;
        cairo_region_t *outside_region;
        cairo_region_t *monitor_region;

        outside_region = get_outside_region (window);

        gdk_screen_get_monitor_geometry (gtk_window_get_screen (GTK_WINDOW (window)),
                                         window->priv->monitor,
                                         &geometry);
        gs_debug ("got geometry for monitor %d: x=%d y=%d w=%d h=%d",
                  window->priv->monitor,
                  geometry.x,
                  geometry.y,
                  geometry.width,
                  geometry.height);
        monitor_region = cairo_region_create_rectangle ((const cairo_rectangle_int_t *)&geometry);
        cairo_region_subtract (monitor_region, outside_region);
        cairo_region_destroy (outside_region);

        cairo_region_get_extents (monitor_region, (cairo_rectangle_int_t *)&geometry);
        cairo_region_destroy (monitor_region);

        gs_debug ("using geometry for monitor %d: x=%d y=%d w=%d h=%d",
                  window->priv->monitor,
                  geometry.x,
                  geometry.y,
                  geometry.width,
                  geometry.height);

        window->priv->geometry.x = geometry.x;
        window->priv->geometry.y = geometry.y;
        window->priv->geometry.width = geometry.width;
        window->priv->geometry.height = geometry.height;
}