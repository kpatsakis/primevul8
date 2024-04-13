static int __init evdev_init(void)
{
	return input_register_handler(&evdev_handler);
}