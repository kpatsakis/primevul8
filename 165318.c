gs_window_real_motion_notify_event (GtkWidget      *widget,
                                    GdkEventMotion *event)
{
        GSWindow  *window;
        gdouble    distance;
        gdouble    min_distance;
        gdouble    min_percentage = 0.1;
        GdkScreen *screen;

        window = GS_WINDOW (widget);

        screen = gs_window_get_screen (window);
        min_distance = gdk_screen_get_width (screen) * min_percentage;

        /* if the last position was not set then don't detect motion */
        if (window->priv->last_x < 0 || window->priv->last_y < 0) {
                window->priv->last_x = event->x;
                window->priv->last_y = event->y;

                return FALSE;
        }

        /* just an approximate distance */
        distance = MAX (ABS (window->priv->last_x - event->x),
                        ABS (window->priv->last_y - event->y));

        if (distance > min_distance) {
                maybe_handle_activity (window);

                window->priv->last_x = -1;
                window->priv->last_y = -1;
        }

        return FALSE;
}