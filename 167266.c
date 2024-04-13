static u32 _usb_read_sync(struct rtl_priv *rtlpriv, u32 addr, u16 len)
{
	struct device *dev = rtlpriv->io.dev;
	struct usb_device *udev = to_usb_device(dev);
	u8 request;
	u16 wvalue;
	u16 index;
	__le32 *data;
	unsigned long flags;

	spin_lock_irqsave(&rtlpriv->locks.usb_lock, flags);
	if (++rtlpriv->usb_data_index >= RTL_USB_MAX_RX_COUNT)
		rtlpriv->usb_data_index = 0;
	data = &rtlpriv->usb_data[rtlpriv->usb_data_index];
	spin_unlock_irqrestore(&rtlpriv->locks.usb_lock, flags);
	request = REALTEK_USB_VENQT_CMD_REQ;
	index = REALTEK_USB_VENQT_CMD_IDX; /* n/a */

	wvalue = (u16)addr;
	_usbctrl_vendorreq_sync_read(udev, request, wvalue, index, data, len);
	return le32_to_cpu(*data);
}