void qemu_input_handler_bind(QemuInputHandlerState *s,
                             const char *device_id, int head,
                             Error **errp)
{
    QemuConsole *con;
    Error *err = NULL;

    con = qemu_console_lookup_by_device_name(device_id, head, &err);
    if (err) {
        error_propagate(errp, err);
        return;
    }

    s->con = con;
}