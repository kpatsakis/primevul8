xevent_filter (GdkXEvent *xevent,
               GdkEvent  *event,
               GSWindow  *window)
{
        gs_window_xevent (window, xevent);

        return GDK_FILTER_CONTINUE;
}