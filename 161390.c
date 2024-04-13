handle_get_remote_hostname (GdmDBusDisplay        *skeleton,
                            GDBusMethodInvocation *invocation,
                            GdmDisplay            *self)
{
        char *hostname;

        gdm_display_get_remote_hostname (self, &hostname, NULL);

        gdm_dbus_display_complete_get_remote_hostname (skeleton,
                                                       invocation,
                                                       hostname ? hostname : "");

        g_free (hostname);
        return TRUE;
}