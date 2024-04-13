static void _rtl_usb_io_handler_release(struct ieee80211_hw *hw)
{
	struct rtl_priv __maybe_unused *rtlpriv = rtl_priv(hw);

	mutex_destroy(&rtlpriv->io.bb_mutex);
}