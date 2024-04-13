static u16 _usb_read16_sync(struct rtl_priv *rtlpriv, u32 addr)
{
	return (u16)_usb_read_sync(rtlpriv, addr, 2);
}