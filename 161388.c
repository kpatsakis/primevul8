gdm_display_get_property (GObject        *object,
                          guint           prop_id,
                          GValue         *value,
                          GParamSpec     *pspec)
{
        GdmDisplay *self;
        GdmDisplayPrivate *priv;

        self = GDM_DISPLAY (object);
        priv = gdm_display_get_instance_private (self);

        switch (prop_id) {
        case PROP_ID:
                g_value_set_string (value, priv->id);
                break;
        case PROP_STATUS:
                g_value_set_int (value, priv->status);
                break;
        case PROP_SEAT_ID:
                g_value_set_string (value, priv->seat_id);
                break;
        case PROP_SESSION_ID:
                g_value_set_string (value, priv->session_id);
                break;
        case PROP_SESSION_CLASS:
                g_value_set_string (value, priv->session_class);
                break;
        case PROP_SESSION_TYPE:
                g_value_set_string (value, priv->session_type);
                break;
        case PROP_REMOTE_HOSTNAME:
                g_value_set_string (value, priv->remote_hostname);
                break;
        case PROP_X11_DISPLAY_NUMBER:
                g_value_set_int (value, priv->x11_display_number);
                break;
        case PROP_X11_DISPLAY_NAME:
                g_value_set_string (value, priv->x11_display_name);
                break;
        case PROP_X11_COOKIE:
                g_value_set_string (value, priv->x11_cookie);
                break;
        case PROP_X11_AUTHORITY_FILE:
                g_value_take_string (value,
                                     priv->access_file?
                                     gdm_display_access_file_get_path (priv->access_file) : NULL);
                break;
        case PROP_IS_LOCAL:
                g_value_set_boolean (value, priv->is_local);
                break;
        case PROP_IS_CONNECTED:
                g_value_set_boolean (value, priv->xcb_connection != NULL);
                break;
        case PROP_LAUNCH_ENVIRONMENT:
                g_value_set_object (value, priv->launch_environment);
                break;
        case PROP_IS_INITIAL:
                g_value_set_boolean (value, priv->is_initial);
                break;
        case PROP_HAVE_EXISTING_USER_ACCOUNTS:
                g_value_set_boolean (value, priv->have_existing_user_accounts);
                break;
        case PROP_DOING_INITIAL_SETUP:
                g_value_set_boolean (value, priv->doing_initial_setup);
                break;
        case PROP_SESSION_REGISTERED:
                g_value_set_boolean (value, priv->session_registered);
                break;
        case PROP_ALLOW_TIMED_LOGIN:
                g_value_set_boolean (value, priv->allow_timed_login);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}