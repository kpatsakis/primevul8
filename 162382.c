void qemu_input_check_mode_change(void)
{
    static int current_is_absolute;
    int is_absolute;

    is_absolute = qemu_input_is_absolute();

    if (is_absolute != current_is_absolute) {
        trace_input_mouse_mode(is_absolute);
        notifier_list_notify(&mouse_mode_notifiers, NULL);
    }

    current_is_absolute = is_absolute;
}