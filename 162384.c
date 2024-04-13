void hmp_mouse_set(Monitor *mon, const QDict *qdict)
{
    QemuInputHandlerState *s;
    int index = qdict_get_int(qdict, "index");
    int found = 0;

    QTAILQ_FOREACH(s, &handlers, node) {
        if (s->id != index) {
            continue;
        }
        if (!(s->handler->mask & (INPUT_EVENT_MASK_REL |
                                  INPUT_EVENT_MASK_ABS))) {
            error_report("Input device '%s' is not a mouse", s->handler->name);
            return;
        }
        found = 1;
        qemu_input_handler_activate(s);
        break;
    }

    if (!found) {
        error_report("Mouse at index '%d' not found", index);
    }

    qemu_input_check_mode_change();
}