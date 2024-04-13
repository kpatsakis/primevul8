get_root_window (xcb_connection_t *connection,
                 int               screen_number)
{
        xcb_screen_t *screen = NULL;
        xcb_screen_iterator_t iter;

        iter = xcb_setup_roots_iterator (xcb_get_setup (connection));
        while (iter.rem) {
                if (screen_number == 0)
                        screen = iter.data;
                screen_number--;
                xcb_screen_next (&iter);
        }

        if (screen != NULL) {
                return screen->root;
        }

        return XCB_WINDOW_NONE;
}