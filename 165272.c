gs_window_get_property (GObject    *object,
                        guint       prop_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
        GSWindow *self;

        self = GS_WINDOW (object);

        switch (prop_id) {
        case PROP_LOCK_ENABLED:
                g_value_set_boolean (value, self->priv->lock_enabled);
                break;
        case PROP_KEYBOARD_ENABLED:
                g_value_set_boolean (value, self->priv->keyboard_enabled);
                break;
        case PROP_KEYBOARD_COMMAND:
                g_value_set_string (value, self->priv->keyboard_command);
                break;
        case PROP_LOGOUT_ENABLED:
                g_value_set_boolean (value, self->priv->logout_enabled);
                break;
        case PROP_LOGOUT_COMMAND:
                g_value_set_string (value, self->priv->logout_command);
                break;
        case PROP_LOGOUT_TIMEOUT:
                g_value_set_long (value, self->priv->logout_timeout);
                break;
        case PROP_MONITOR:
                g_value_set_int (value, self->priv->monitor);
                break;
        case PROP_OBSCURED:
                g_value_set_boolean (value, self->priv->obscured);
                break;
        case PROP_DIALOG_UP:
                g_value_set_boolean (value, self->priv->dialog_up);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}