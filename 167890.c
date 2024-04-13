main(int argc, char *argv[])
{
    GOptionContext *context;
    GError *error = NULL;
    GMainLoop *loop = NULL;
    int fd;
    VuGpu g = { .sock_fd = -1, .drm_rnode_fd = -1 };

    QTAILQ_INIT(&g.reslist);
    QTAILQ_INIT(&g.fenceq);

    context = g_option_context_new("QEMU vhost-user-gpu");
    g_option_context_add_main_entries(context, entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_printerr("Option parsing failed: %s\n", error->message);
        exit(EXIT_FAILURE);
    }
    g_option_context_free(context);

    if (opt_print_caps) {
        g_print("{\n");
        g_print("  \"type\": \"gpu\",\n");
        g_print("  \"features\": [\n");
        g_print("    \"render-node\",\n");
        g_print("    \"virgl\"\n");
        g_print("  ]\n");
        g_print("}\n");
        exit(EXIT_SUCCESS);
    }

    g.drm_rnode_fd = qemu_drm_rendernode_open(opt_render_node);
    if (opt_render_node && g.drm_rnode_fd == -1) {
        g_printerr("Failed to open DRM rendernode.\n");
        exit(EXIT_FAILURE);
    }

    vugbm_device_init(&g.gdev, g.drm_rnode_fd);

    if ((!!opt_socket_path + (opt_fdnum != -1)) != 1) {
        g_printerr("Please specify either --fd or --socket-path\n");
        exit(EXIT_FAILURE);
    }

    if (opt_socket_path) {
        int lsock = unix_listen(opt_socket_path, &error_fatal);
        if (lsock < 0) {
            g_printerr("Failed to listen on %s.\n", opt_socket_path);
            exit(EXIT_FAILURE);
        }
        fd = accept(lsock, NULL, NULL);
        close(lsock);
    } else {
        fd = opt_fdnum;
    }
    if (fd == -1) {
        g_printerr("Invalid vhost-user socket.\n");
        exit(EXIT_FAILURE);
    }

    if (!vug_init(&g.dev, VHOST_USER_GPU_MAX_QUEUES, fd, vg_panic, &vuiface)) {
        g_printerr("Failed to initialize libvhost-user-glib.\n");
        exit(EXIT_FAILURE);
    }

    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
    g_main_loop_unref(loop);

    vg_destroy(&g);
    if (g.drm_rnode_fd >= 0) {
        close(g.drm_rnode_fd);
    }

    return 0;
}