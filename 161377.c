_gdm_display_set_status (GdmDisplay *self,
                         int         status)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        if (status != priv->status) {
                priv->status = status;
                g_object_notify (G_OBJECT (self), "status");
        }
}