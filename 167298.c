static u32 _usb_read32_sync(struct rtl_priv *rtlpriv, u32 addr)
{
	return _usb_read_sync(rtlpriv, addr, 4);
}