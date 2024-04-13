static bool is_marantz_denon_dac(unsigned int id)
{
	switch (id) {
	case USB_ID(0x154e, 0x1003): /* Denon DA-300USB */
	case USB_ID(0x154e, 0x3005): /* Marantz HD-DAC1 */
	case USB_ID(0x154e, 0x3006): /* Marantz SA-14S1 */
		return true;
	}
	return false;
}