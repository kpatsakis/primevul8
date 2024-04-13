QemuInputHandlerState *qemu_input_handler_register(DeviceState *dev,
                                                   QemuInputHandler *handler)
{
    QemuInputHandlerState *s = g_new0(QemuInputHandlerState, 1);
    static int id = 1;

    s->dev = dev;
    s->handler = handler;
    s->id = id++;
    QTAILQ_INSERT_TAIL(&handlers, s, node);

    qemu_input_check_mode_change();
    return s;
}