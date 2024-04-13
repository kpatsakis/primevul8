wants_initial_setup (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;
        gboolean enabled = FALSE;
        gboolean forced = FALSE;

        priv = gdm_display_get_instance_private (self);

        if (already_done_initial_setup_on_this_boot ()) {
                return FALSE;
        }

        if (kernel_cmdline_initial_setup_force_state (&forced)) {
                if (forced) {
                        g_debug ("GdmDisplay: Forcing gnome-initial-setup");
                        return TRUE;
                }

                g_debug ("GdmDisplay: Forcing no gnome-initial-setup");
                return FALSE;
        }

        /* don't run initial-setup on remote displays
         */
        if (!priv->is_local) {
                return FALSE;
        }

        /* don't run if the system has existing users */
        if (priv->have_existing_user_accounts) {
                return FALSE;
        }

        /* don't run if initial-setup is unavailable */
        if (!can_create_environment ("gnome-initial-setup")) {
                return FALSE;
        }

        if (!gdm_settings_direct_get_boolean (GDM_KEY_INITIAL_SETUP_ENABLE, &enabled)) {
                return FALSE;
        }

        return enabled;
}