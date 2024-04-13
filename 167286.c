static void _usb_write8_async(struct rtl_priv *rtlpriv, u32 addr, u8 val)
{
	struct device *dev = rtlpriv->io.dev;

	_usb_write_async(to_usb_device(dev), addr, val, 1);
}