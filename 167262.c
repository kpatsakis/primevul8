static void _rtl_usb_io_handler_init(struct device *dev,
				     struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	rtlpriv->io.dev = dev;
	mutex_init(&rtlpriv->io.bb_mutex);
	rtlpriv->io.write8_async	= _usb_write8_async;
	rtlpriv->io.write16_async	= _usb_write16_async;
	rtlpriv->io.write32_async	= _usb_write32_async;
	rtlpriv->io.read8_sync		= _usb_read8_sync;
	rtlpriv->io.read16_sync		= _usb_read16_sync;
	rtlpriv->io.read32_sync		= _usb_read32_sync;
	rtlpriv->io.writen_sync		= _usb_writen_sync;
}