gdm_display_dispose (GObject *object)
{
        GdmDisplay *self;
        GdmDisplayPrivate *priv;

        self = GDM_DISPLAY (object);
        priv = gdm_display_get_instance_private (self);

        g_debug ("GdmDisplay: Disposing display");

        if (priv->finish_idle_id != 0) {
                g_source_remove (priv->finish_idle_id);
                priv->finish_idle_id = 0;
        }
        g_clear_object (&priv->launch_environment);

        g_warn_if_fail (priv->status != GDM_DISPLAY_MANAGED);
        g_warn_if_fail (priv->user_access_file == NULL);
        g_warn_if_fail (priv->access_file == NULL);

        G_OBJECT_CLASS (gdm_display_parent_class)->dispose (object);
}