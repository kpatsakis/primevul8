gs_window_set_property (GObject            *object,
                        guint               prop_id,
                        const GValue       *value,
                        GParamSpec         *pspec)
{
        GSWindow *self;

        self = GS_WINDOW (object);

        switch (prop_id) {
        case PROP_LOCK_ENABLED:
                gs_window_set_lock_enabled (self, g_value_get_boolean (value));
                break;
        case PROP_KEYBOARD_ENABLED:
                gs_window_set_keyboard_enabled (self, g_value_get_boolean (value));
                break;
        case PROP_KEYBOARD_COMMAND:
                gs_window_set_keyboard_command (self, g_value_get_string (value));
                break;
        case PROP_LOGOUT_ENABLED:
                gs_window_set_logout_enabled (self, g_value_get_boolean (value));
                break;
        case PROP_LOGOUT_COMMAND:
                gs_window_set_logout_command (self, g_value_get_string (value));
                break;
        case PROP_LOGOUT_TIMEOUT:
                gs_window_set_logout_timeout (self, g_value_get_long (value));
                break;
        case PROP_MONITOR:
                gs_window_set_monitor (self, g_value_get_int (value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}