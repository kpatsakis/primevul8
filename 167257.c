static u8 _usb_read8_sync(struct rtl_priv *rtlpriv, u32 addr)
{
	return (u8)_usb_read_sync(rtlpriv, addr, 1);
}