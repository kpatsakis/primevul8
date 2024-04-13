set_invisible_cursor (GdkWindow *window,
                      gboolean   invisible)
{
        GdkCursor *cursor = NULL;

        if (invisible) {
                cursor = gdk_cursor_new (GDK_BLANK_CURSOR);
        }

        gdk_window_set_cursor (window, cursor);

        if (cursor) {
                gdk_cursor_unref (cursor);
        }
}