gdm_display_get_object_skeleton (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        return priv->object_skeleton;
}