gdm_display_get_status (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), 0);

        priv = gdm_display_get_instance_private (self);
        return priv->status;
}