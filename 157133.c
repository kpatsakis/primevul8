	snd_usb_find_output_terminal_descriptor(struct usb_host_interface *ctrl_iface,
						int terminal_id)
{
	struct uac2_output_terminal_descriptor *term = NULL;

	while ((term = snd_usb_find_csint_desc(ctrl_iface->extra,
					       ctrl_iface->extralen,
					       term, UAC_OUTPUT_TERMINAL))) {
		if (term->bTerminalID == terminal_id)
			return term;
	}

	return NULL;
}