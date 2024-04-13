static int __check_input_term(struct mixer_build *state, int id,
			    struct usb_audio_term *term)
{
	int protocol = state->mixer->protocol;
	int err;
	void *p1;
	unsigned char *hdr;

	memset(term, 0, sizeof(*term));
	for (;;) {
		/* a loop in the terminal chain? */
		if (test_and_set_bit(id, state->termbitmap))
			return -EINVAL;

		p1 = find_audio_control_unit(state, id);
		if (!p1)
			break;

		hdr = p1;
		term->id = id;

		if (protocol == UAC_VERSION_1 || protocol == UAC_VERSION_2) {
			switch (hdr[2]) {
			case UAC_INPUT_TERMINAL:
				if (protocol == UAC_VERSION_1) {
					struct uac_input_terminal_descriptor *d = p1;

					term->type = le16_to_cpu(d->wTerminalType);
					term->channels = d->bNrChannels;
					term->chconfig = le16_to_cpu(d->wChannelConfig);
					term->name = d->iTerminal;
				} else { /* UAC_VERSION_2 */
					struct uac2_input_terminal_descriptor *d = p1;

					/* call recursively to verify that the
					 * referenced clock entity is valid */
					err = __check_input_term(state, d->bCSourceID, term);
					if (err < 0)
						return err;

					/* save input term properties after recursion,
					 * to ensure they are not overriden by the
					 * recursion calls */
					term->id = id;
					term->type = le16_to_cpu(d->wTerminalType);
					term->channels = d->bNrChannels;
					term->chconfig = le32_to_cpu(d->bmChannelConfig);
					term->name = d->iTerminal;
				}
				return 0;
			case UAC_FEATURE_UNIT: {
				/* the header is the same for v1 and v2 */
				struct uac_feature_unit_descriptor *d = p1;

				id = d->bSourceID;
				break; /* continue to parse */
			}
			case UAC_MIXER_UNIT: {
				struct uac_mixer_unit_descriptor *d = p1;

				term->type = UAC3_MIXER_UNIT << 16; /* virtual type */
				term->channels = uac_mixer_unit_bNrChannels(d);
				term->chconfig = uac_mixer_unit_wChannelConfig(d, protocol);
				term->name = uac_mixer_unit_iMixer(d);
				return 0;
			}
			case UAC_SELECTOR_UNIT:
			case UAC2_CLOCK_SELECTOR: {
				struct uac_selector_unit_descriptor *d = p1;
				/* call recursively to retrieve the channel info */
				err = __check_input_term(state, d->baSourceID[0], term);
				if (err < 0)
					return err;
				term->type = UAC3_SELECTOR_UNIT << 16; /* virtual type */
				term->id = id;
				term->name = uac_selector_unit_iSelector(d);
				return 0;
			}
			case UAC1_PROCESSING_UNIT:
			/* UAC2_EFFECT_UNIT */
				if (protocol == UAC_VERSION_1)
					term->type = UAC3_PROCESSING_UNIT << 16; /* virtual type */
				else /* UAC_VERSION_2 */
					term->type = UAC3_EFFECT_UNIT << 16; /* virtual type */
				/* fall through */
			case UAC1_EXTENSION_UNIT:
			/* UAC2_PROCESSING_UNIT_V2 */
				if (protocol == UAC_VERSION_1 && !term->type)
					term->type = UAC3_EXTENSION_UNIT << 16; /* virtual type */
				else if (protocol == UAC_VERSION_2 && !term->type)
					term->type = UAC3_PROCESSING_UNIT << 16; /* virtual type */
				/* fall through */
			case UAC2_EXTENSION_UNIT_V2: {
				struct uac_processing_unit_descriptor *d = p1;

				if (protocol == UAC_VERSION_2 &&
					hdr[2] == UAC2_EFFECT_UNIT) {
					/* UAC2/UAC1 unit IDs overlap here in an
					 * uncompatible way. Ignore this unit for now.
					 */
					return 0;
				}

				if (d->bNrInPins) {
					id = d->baSourceID[0];
					break; /* continue to parse */
				}
				if (!term->type)
					term->type = UAC3_EXTENSION_UNIT << 16; /* virtual type */

				term->channels = uac_processing_unit_bNrChannels(d);
				term->chconfig = uac_processing_unit_wChannelConfig(d, protocol);
				term->name = uac_processing_unit_iProcessing(d, protocol);
				return 0;
			}
			case UAC2_CLOCK_SOURCE: {
				struct uac_clock_source_descriptor *d = p1;

				term->type = UAC3_CLOCK_SOURCE << 16; /* virtual type */
				term->id = id;
				term->name = d->iClockSource;
				return 0;
			}
			default:
				return -ENODEV;
			}
		} else { /* UAC_VERSION_3 */
			switch (hdr[2]) {
			case UAC_INPUT_TERMINAL: {
				struct uac3_input_terminal_descriptor *d = p1;

				/* call recursively to verify that the
				 * referenced clock entity is valid */
				err = __check_input_term(state, d->bCSourceID, term);
				if (err < 0)
					return err;

				/* save input term properties after recursion,
				 * to ensure they are not overriden by the
				 * recursion calls */
				term->id = id;
				term->type = le16_to_cpu(d->wTerminalType);

				err = get_cluster_channels_v3(state, le16_to_cpu(d->wClusterDescrID));
				if (err < 0)
					return err;
				term->channels = err;

				/* REVISIT: UAC3 IT doesn't have channels cfg */
				term->chconfig = 0;

				term->name = le16_to_cpu(d->wTerminalDescrStr);
				return 0;
			}
			case UAC3_FEATURE_UNIT: {
				struct uac3_feature_unit_descriptor *d = p1;

				id = d->bSourceID;
				break; /* continue to parse */
			}
			case UAC3_CLOCK_SOURCE: {
				struct uac3_clock_source_descriptor *d = p1;

				term->type = UAC3_CLOCK_SOURCE << 16; /* virtual type */
				term->id = id;
				term->name = le16_to_cpu(d->wClockSourceStr);
				return 0;
			}
			case UAC3_MIXER_UNIT: {
				struct uac_mixer_unit_descriptor *d = p1;

				err = uac_mixer_unit_get_channels(state, d);
				if (err <= 0)
					return err;

				term->channels = err;
				term->type = UAC3_MIXER_UNIT << 16; /* virtual type */

				return 0;
			}
			case UAC3_SELECTOR_UNIT:
			case UAC3_CLOCK_SELECTOR: {
				struct uac_selector_unit_descriptor *d = p1;
				/* call recursively to retrieve the channel info */
				err = __check_input_term(state, d->baSourceID[0], term);
				if (err < 0)
					return err;
				term->type = UAC3_SELECTOR_UNIT << 16; /* virtual type */
				term->id = id;
				term->name = 0; /* TODO: UAC3 Class-specific strings */

				return 0;
			}
			case UAC3_PROCESSING_UNIT: {
				struct uac_processing_unit_descriptor *d = p1;

				if (!d->bNrInPins)
					return -EINVAL;

				/* call recursively to retrieve the channel info */
				err = __check_input_term(state, d->baSourceID[0], term);
				if (err < 0)
					return err;

				term->type = UAC3_PROCESSING_UNIT << 16; /* virtual type */
				term->id = id;
				term->name = 0; /* TODO: UAC3 Class-specific strings */

				return 0;
			}
			default:
				return -ENODEV;
			}
		}
	}
	return -ENODEV;
}