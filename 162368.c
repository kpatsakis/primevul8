void qemu_remove_mouse_mode_change_notifier(Notifier *notify)
{
    notifier_remove(notify);
}