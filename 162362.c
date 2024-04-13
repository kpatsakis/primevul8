void qemu_input_handler_unregister(QemuInputHandlerState *s)
{
    QTAILQ_REMOVE(&handlers, s, node);
    g_free(s);
    qemu_input_check_mode_change();
}