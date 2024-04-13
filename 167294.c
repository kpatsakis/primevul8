static int _usbctrl_vendorreq_sync_read(struct usb_device *udev, u8 request,
					u16 value, u16 index, void *pdata,
					u16 len)
{
	unsigned int pipe;
	int status;
	u8 reqtype;
	int vendorreq_times = 0;
	static int count;

	pipe = usb_rcvctrlpipe(udev, 0); /* read_in */
	reqtype =  REALTEK_USB_VENQT_READ;

	do {
		status = usb_control_msg(udev, pipe, request, reqtype, value,
					 index, pdata, len, 1000);
		if (status < 0) {
			/* firmware download is checksumed, don't retry */
			if ((value >= FW_8192C_START_ADDRESS &&
			    value <= FW_8192C_END_ADDRESS))
				break;
		} else {
			break;
		}
	} while (++vendorreq_times < MAX_USBCTRL_VENDORREQ_TIMES);

	if (status < 0 && count++ < 4)
		pr_err("reg 0x%x, usbctrl_vendorreq TimeOut! status:0x%x value=0x%x\n",
		       value, status, *(u32 *)pdata);
	return status;
}