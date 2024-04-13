gs_window_new (GdkScreen *screen,
               int        monitor,
               gboolean   lock_enabled)
{
        GObject     *result;

        result = g_object_new (GS_TYPE_WINDOW,
                               "type", GTK_WINDOW_POPUP,
                               "screen", screen,
                               "monitor", monitor,
                               "lock-enabled", lock_enabled,
                               "app-paintable", TRUE,
                               NULL);

        return GS_WINDOW (result);
}