x11_window_is_ours (Window window)
{
        GdkWindow *gwindow;
        gboolean   ret;

        ret = FALSE;

        gwindow = gdk_x11_window_lookup_for_display (gdk_display_get_default (), window);
        if (gwindow && (window != GDK_ROOT_WINDOW ())) {
                ret = TRUE;
        }

        return ret;
}