int unregister_keyboard_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&keyboard_notifier_list, nb);
}