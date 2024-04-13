on_launch_environment_session_died (GdmLaunchEnvironment *launch_environment,
                                    int                   signal,
                                    GdmDisplay           *self)
{
        g_debug ("GdmDisplay: Greeter died: %d", signal);
        self_destruct (self);
}