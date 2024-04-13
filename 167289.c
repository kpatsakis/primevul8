static void _usb_writen_sync(struct rtl_priv *rtlpriv, u32 addr, void *data,
			     u16 len)
{
	struct device *dev = rtlpriv->io.dev;
	struct usb_device *udev = to_usb_device(dev);
	u8 request = REALTEK_USB_VENQT_CMD_REQ;
	u8 reqtype =  REALTEK_USB_VENQT_WRITE;
	u16 wvalue;
	u16 index = REALTEK_USB_VENQT_CMD_IDX;
	int pipe = usb_sndctrlpipe(udev, 0); /* write_out */
	u8 *buffer;

	wvalue = (u16)(addr & 0x0000ffff);
	buffer = kmemdup(data, len, GFP_ATOMIC);
	if (!buffer)
		return;
	usb_control_msg(udev, pipe, request, reqtype, wvalue,
			index, buffer, len, 50);

	kfree(buffer);
}