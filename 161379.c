handle_is_local (GdmDBusDisplay        *skeleton,
                 GDBusMethodInvocation *invocation,
                 GdmDisplay            *self)
{
        gboolean is_local;

        gdm_display_is_local (self, &is_local, NULL);

        gdm_dbus_display_complete_is_local (skeleton, invocation, is_local);

        return TRUE;
}