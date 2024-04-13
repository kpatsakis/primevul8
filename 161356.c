gdm_display_class_init (GdmDisplayClass *klass)
{
        GObjectClass   *object_class = G_OBJECT_CLASS (klass);

        object_class->get_property = gdm_display_get_property;
        object_class->set_property = gdm_display_set_property;
        object_class->constructor = gdm_display_constructor;
        object_class->dispose = gdm_display_dispose;
        object_class->finalize = gdm_display_finalize;

        klass->prepare = gdm_display_real_prepare;

        g_object_class_install_property (object_class,
                                         PROP_ID,
                                         g_param_spec_string ("id",
                                                              "id",
                                                              "id",
                                                              NULL,
                                                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_REMOTE_HOSTNAME,
                                         g_param_spec_string ("remote-hostname",
                                                              "remote-hostname",
                                                              "remote-hostname",
                                                              NULL,
                                                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_X11_DISPLAY_NUMBER,
                                         g_param_spec_int ("x11-display-number",
                                                          "x11 display number",
                                                          "x11 display number",
                                                          -1,
                                                          G_MAXINT,
                                                          -1,
                                                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_X11_DISPLAY_NAME,
                                         g_param_spec_string ("x11-display-name",
                                                              "x11-display-name",
                                                              "x11-display-name",
                                                              NULL,
                                                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_SEAT_ID,
                                         g_param_spec_string ("seat-id",
                                                              "seat id",
                                                              "seat id",
                                                              NULL,
                                                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_SESSION_ID,
                                         g_param_spec_string ("session-id",
                                                              "session id",
                                                              "session id",
                                                              NULL,
                                                              G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_SESSION_CLASS,
                                         g_param_spec_string ("session-class",
                                                              NULL,
                                                              NULL,
                                                              "greeter",
                                                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_SESSION_TYPE,
                                         g_param_spec_string ("session-type",
                                                              NULL,
                                                              NULL,
                                                              NULL,
                                                              G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_IS_INITIAL,
                                         g_param_spec_boolean ("is-initial",
                                                               NULL,
                                                               NULL,
                                                               FALSE,
                                                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_ALLOW_TIMED_LOGIN,
                                         g_param_spec_boolean ("allow-timed-login",
                                                               NULL,
                                                               NULL,
                                                               TRUE,
                                                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_X11_COOKIE,
                                         g_param_spec_string ("x11-cookie",
                                                              "cookie",
                                                              "cookie",
                                                              NULL,
                                                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_X11_AUTHORITY_FILE,
                                         g_param_spec_string ("x11-authority-file",
                                                              "authority file",
                                                              "authority file",
                                                              NULL,
                                                              G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

        g_object_class_install_property (object_class,
                                         PROP_IS_LOCAL,
                                         g_param_spec_boolean ("is-local",
                                                               NULL,
                                                               NULL,
                                                               TRUE,
                                                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_IS_CONNECTED,
                                         g_param_spec_boolean ("is-connected",
                                                               NULL,
                                                               NULL,
                                                               TRUE,
                                                               G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_HAVE_EXISTING_USER_ACCOUNTS,
                                         g_param_spec_boolean ("have-existing-user-accounts",
                                                               NULL,
                                                               NULL,
                                                               FALSE,
                                                               G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_DOING_INITIAL_SETUP,
                                         g_param_spec_boolean ("doing-initial-setup",
                                                               NULL,
                                                               NULL,
                                                               FALSE,
                                                               G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_SESSION_REGISTERED,
                                         g_param_spec_boolean ("session-registered",
                                                               NULL,
                                                               NULL,
                                                               FALSE,
                                                               G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

        g_object_class_install_property (object_class,
                                         PROP_LAUNCH_ENVIRONMENT,
                                         g_param_spec_object ("launch-environment",
                                                              NULL,
                                                              NULL,
                                                              GDM_TYPE_LAUNCH_ENVIRONMENT,
                                                              G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
        g_object_class_install_property (object_class,
                                         PROP_STATUS,
                                         g_param_spec_int ("status",
                                                           "status",
                                                           "status",
                                                           -1,
                                                           G_MAXINT,
                                                           GDM_DISPLAY_UNMANAGED,
                                                           G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS));
}