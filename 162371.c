void qmp_input_send_event(bool has_device, const char *device,
                          bool has_head, int64_t head,
                          InputEventList *events, Error **errp)
{
    InputEventList *e;
    QemuConsole *con;
    Error *err = NULL;

    con = NULL;
    if (has_device) {
        if (!has_head) {
            head = 0;
        }
        con = qemu_console_lookup_by_device_name(device, head, &err);
        if (err) {
            error_propagate(errp, err);
            return;
        }
    }

    if (!runstate_is_running() && !runstate_check(RUN_STATE_SUSPENDED)) {
        error_setg(errp, "VM not running");
        return;
    }

    for (e = events; e != NULL; e = e->next) {
        InputEvent *event = e->value;

        if (!qemu_input_find_handler(1 << event->type, con)) {
            error_setg(errp, "Input handler not found for "
                             "event type %s",
                            InputEventKind_lookup[event->type]);
            return;
        }
    }

    for (e = events; e != NULL; e = e->next) {
        InputEvent *event = e->value;

        qemu_input_event_send(con, event);
    }

    qemu_input_event_sync();
}