gdm_display_finish (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);
        if (priv->finish_idle_id != 0) {
                g_source_remove (priv->finish_idle_id);
                priv->finish_idle_id = 0;
        }

        _gdm_display_set_status (self, GDM_DISPLAY_FINISHED);

        g_debug ("GdmDisplay: finish display");

        return TRUE;
}