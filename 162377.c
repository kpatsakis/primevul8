void qemu_add_mouse_mode_change_notifier(Notifier *notify)
{
    notifier_list_add(&mouse_mode_notifiers, notify);
}