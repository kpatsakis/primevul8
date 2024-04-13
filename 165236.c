gs_window_set_background_surface (GSWindow        *window,
                                  cairo_surface_t *surface)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        if (window->priv->background_surface != NULL) {
                cairo_surface_destroy (window->priv->background_surface);
        }

        if (surface != NULL) {
                window->priv->background_surface = cairo_surface_reference (surface);
                gs_window_reset_background_surface (window);
        }
}