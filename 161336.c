gdm_display_connect (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;
        xcb_auth_info_t *auth_info = NULL;
        gboolean ret;

        priv = gdm_display_get_instance_private (self);
        ret = FALSE;

        g_debug ("GdmDisplay: Server is ready - opening display %s", priv->x11_display_name);

        /* Get access to the display independent of current hostname */
        if (priv->x11_cookie != NULL) {
                auth_info = g_alloca (sizeof (xcb_auth_info_t));

                auth_info->namelen = strlen ("MIT-MAGIC-COOKIE-1");
                auth_info->name = "MIT-MAGIC-COOKIE-1";
                auth_info->datalen = priv->x11_cookie_size;
                auth_info->data = priv->x11_cookie;

        }

        priv->xcb_connection = xcb_connect_to_display_with_auth_info (priv->x11_display_name,
                                                                      auth_info,
                                                                      &priv->xcb_screen_number);

        if (xcb_connection_has_error (priv->xcb_connection)) {
                g_clear_pointer (&priv->xcb_connection, xcb_disconnect);
                g_warning ("Unable to connect to display %s", priv->x11_display_name);
                ret = FALSE;
        } else if (priv->is_local) {
                XHostAddress              host_entries[3];
                xcb_void_cookie_t         cookies[3];
                int                       i;

                g_debug ("GdmDisplay: Connected to display %s", priv->x11_display_name);
                ret = TRUE;

                /* Give programs access to the display independent of current hostname
                 */
                setup_xhost_auth (host_entries);

                for (i = 0; i < G_N_ELEMENTS (host_entries); i++) {
                        cookies[i] = xcb_change_hosts_checked (priv->xcb_connection,
                                                               XCB_HOST_MODE_INSERT,
                                                               host_entries[i].family,
                                                               host_entries[i].length,
                                                               (uint8_t *) host_entries[i].address);
                }

                for (i = 0; i < G_N_ELEMENTS (cookies); i++) {
                        xcb_generic_error_t *xcb_error;

                        xcb_error = xcb_request_check (priv->xcb_connection, cookies[i]);

                        if (xcb_error != NULL) {
                                g_debug ("Failed to give system user '%s' access to the display. Trying to proceed.", host_entries[i].address + sizeof ("localuser"));
                                free (xcb_error);
                        } else {
                                g_debug ("Gave system user '%s' access to the display.", host_entries[i].address + sizeof ("localuser"));
                        }
                }

                gdm_display_set_windowpath (self);
        } else {
                g_debug ("GdmDisplay: Connected to display %s", priv->x11_display_name);
                ret = TRUE;
        }

        if (ret == TRUE) {
                g_object_notify (G_OBJECT (self), "is-connected");
        }

        return ret;
}