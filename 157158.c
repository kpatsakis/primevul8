static int snd_usb_fasttrackpro_boot_quirk(struct usb_device *dev)
{
	int err;

	if (dev->actconfig->desc.bConfigurationValue == 1) {
		dev_info(&dev->dev,
			   "Fast Track Pro switching to config #2\n");
		/* This function has to be available by the usb core module.
		 * if it is not avialable the boot quirk has to be left out
		 * and the configuration has to be set by udev or hotplug
		 * rules
		 */
		err = usb_driver_set_configuration(dev, 2);
		if (err < 0)
			dev_dbg(&dev->dev,
				"error usb_driver_set_configuration: %d\n",
				err);
		/* Always return an error, so that we stop creating a device
		   that will just be destroyed and recreated with a new
		   configuration */
		return -ENODEV;
	} else
		dev_info(&dev->dev, "Fast Track Pro config OK\n");

	return 0;
}