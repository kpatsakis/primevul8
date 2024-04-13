gdm_display_get_session_id (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        return priv->session_id;
}