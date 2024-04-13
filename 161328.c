gdm_display_finalize (GObject *object)
{
        GdmDisplay *self;
        GdmDisplayPrivate *priv;

        g_return_if_fail (object != NULL);
        g_return_if_fail (GDM_IS_DISPLAY (object));

        self = GDM_DISPLAY (object);
        priv = gdm_display_get_instance_private (self);

        g_return_if_fail (priv != NULL);

        g_debug ("GdmDisplay: Finalizing display: %s", priv->id);
        g_free (priv->id);
        g_free (priv->seat_id);
        g_free (priv->session_class);
        g_free (priv->remote_hostname);
        g_free (priv->x11_display_name);
        g_free (priv->x11_cookie);

        g_clear_object (&priv->display_skeleton);
        g_clear_object (&priv->object_skeleton);
        g_clear_object (&priv->connection);
        g_clear_object (&priv->accountsservice_proxy);

        if (priv->access_file != NULL) {
                g_object_unref (priv->access_file);
        }

        if (priv->user_access_file != NULL) {
                g_object_unref (priv->user_access_file);
        }

        G_OBJECT_CLASS (gdm_display_parent_class)->finalize (object);
}