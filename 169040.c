int register_keyboard_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&keyboard_notifier_list, nb);
}