_gdm_display_set_launch_environment (GdmDisplay           *self,
                                     GdmLaunchEnvironment *launch_environment)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);

        g_clear_object (&priv->launch_environment);

        priv->launch_environment = g_object_ref (launch_environment);
}