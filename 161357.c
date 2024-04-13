gdm_display_set_windowpath (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;
        /* setting WINDOWPATH for clients */
        xcb_intern_atom_cookie_t atom_cookie;
        xcb_intern_atom_reply_t *atom_reply = NULL;
        xcb_get_property_cookie_t get_property_cookie;
        xcb_get_property_reply_t *get_property_reply = NULL;
        xcb_window_t root_window = XCB_WINDOW_NONE;
        const char *windowpath;
        char *newwindowpath;
        uint32_t num;
        char nums[10];
        int numn;

        priv = gdm_display_get_instance_private (self);

        atom_cookie = xcb_intern_atom (priv->xcb_connection, 0, strlen("XFree86_VT"), "XFree86_VT");
        atom_reply = xcb_intern_atom_reply (priv->xcb_connection, atom_cookie, NULL);

        if (atom_reply == NULL) {
                g_debug ("no XFree86_VT atom\n");
                goto out;
        }

        root_window = get_root_window (priv->xcb_connection,
                                       priv->xcb_screen_number);

        if (root_window == XCB_WINDOW_NONE) {
                g_debug ("couldn't find root window\n");
                goto out;
        }

        get_property_cookie = xcb_get_property (priv->xcb_connection,
                                                FALSE,
                                                root_window,
                                                atom_reply->atom,
                                                XCB_ATOM_INTEGER,
                                                0,
                                                1);

        get_property_reply = xcb_get_property_reply (priv->xcb_connection, get_property_cookie, NULL);

        if (get_property_reply == NULL) {
                g_debug ("no XFree86_VT property\n");
                goto out;
        }

        num = ((uint32_t *) xcb_get_property_value (get_property_reply))[0];

        windowpath = getenv ("WINDOWPATH");
        numn = snprintf (nums, sizeof (nums), "%u", num);
        if (!windowpath) {
                newwindowpath = malloc (numn + 1);
                sprintf (newwindowpath, "%s", nums);
        } else {
                newwindowpath = malloc (strlen (windowpath) + 1 + numn + 1);
                sprintf (newwindowpath, "%s:%s", windowpath, nums);
        }

        g_setenv ("WINDOWPATH", newwindowpath, TRUE);
out:
        g_clear_pointer (&atom_reply, free);
        g_clear_pointer (&get_property_reply, free);
}