static void fix_crc_bug(struct urb *urb, __le16 max_packet_size)
{
	static const u8 crc_check[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
	u32 rest = urb->actual_length % le16_to_cpu(max_packet_size);

	if (((rest == 5) || (rest == 6)) &&
	    !memcmp(((u8 *)urb->transfer_buffer) + urb->actual_length - 4,
		    crc_check, 4)) {
		urb->actual_length -= 4;
	}
}