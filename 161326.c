gdm_display_prepare (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;
        gboolean ret;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);

        g_debug ("GdmDisplay: Preparing display: %s", priv->id);

        /* FIXME: we should probably do this in a more global place,
         * asynchronously
         */
        if (!look_for_existing_users_sync (self)) {
                exit (EXIT_FAILURE);
        }

        priv->doing_initial_setup = wants_initial_setup (self);

        g_object_ref (self);
        ret = GDM_DISPLAY_GET_CLASS (self)->prepare (self);
        g_object_unref (self);

        return ret;
}