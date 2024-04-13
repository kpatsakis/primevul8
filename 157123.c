u64 snd_usb_interface_dsd_format_quirks(struct snd_usb_audio *chip,
					struct audioformat *fp,
					unsigned int sample_bytes)
{
	/* Playback Designs */
	if (USB_ID_VENDOR(chip->usb_id) == 0x23ba) {
		switch (fp->altsetting) {
		case 1:
			fp->dsd_dop = true;
			return SNDRV_PCM_FMTBIT_DSD_U16_LE;
		case 2:
			fp->dsd_bitrev = true;
			return SNDRV_PCM_FMTBIT_DSD_U8;
		case 3:
			fp->dsd_bitrev = true;
			return SNDRV_PCM_FMTBIT_DSD_U16_LE;
		}
	}

	/* XMOS based USB DACs */
	switch (chip->usb_id) {
	case USB_ID(0x20b1, 0x3008): /* iFi Audio micro/nano iDSD */
	case USB_ID(0x20b1, 0x2008): /* Matrix Audio X-Sabre */
	case USB_ID(0x20b1, 0x300a): /* Matrix Audio Mini-i Pro */
	case USB_ID(0x22d9, 0x0416): /* OPPO HA-1 */
		if (fp->altsetting == 2)
			return SNDRV_PCM_FMTBIT_DSD_U32_BE;
		break;

	case USB_ID(0x20b1, 0x000a): /* Gustard DAC-X20U */
	case USB_ID(0x20b1, 0x2009): /* DIYINHK DSD DXD 384kHz USB to I2S/DSD */
	case USB_ID(0x20b1, 0x2023): /* JLsounds I2SoverUSB */
	case USB_ID(0x20b1, 0x3023): /* Aune X1S 32BIT/384 DSD DAC */
	case USB_ID(0x2616, 0x0106): /* PS Audio NuWave DAC */
		if (fp->altsetting == 3)
			return SNDRV_PCM_FMTBIT_DSD_U32_BE;
		break;
	default:
		break;
	}

	/* Denon/Marantz devices with USB DAC functionality */
	if (is_marantz_denon_dac(chip->usb_id)) {
		if (fp->altsetting == 2)
			return SNDRV_PCM_FMTBIT_DSD_U32_BE;
	}

	return 0;
}