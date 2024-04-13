static struct snd_pcm_chmap_elem *convert_chmap(int channels, unsigned int bits,
						int protocol)
{
	static unsigned int uac1_maps[] = {
		SNDRV_CHMAP_FL,		/* left front */
		SNDRV_CHMAP_FR,		/* right front */
		SNDRV_CHMAP_FC,		/* center front */
		SNDRV_CHMAP_LFE,	/* LFE */
		SNDRV_CHMAP_SL,		/* left surround */
		SNDRV_CHMAP_SR,		/* right surround */
		SNDRV_CHMAP_FLC,	/* left of center */
		SNDRV_CHMAP_FRC,	/* right of center */
		SNDRV_CHMAP_RC,		/* surround */
		SNDRV_CHMAP_SL,		/* side left */
		SNDRV_CHMAP_SR,		/* side right */
		SNDRV_CHMAP_TC,		/* top */
		0 /* terminator */
	};
	static unsigned int uac2_maps[] = {
		SNDRV_CHMAP_FL,		/* front left */
		SNDRV_CHMAP_FR,		/* front right */
		SNDRV_CHMAP_FC,		/* front center */
		SNDRV_CHMAP_LFE,	/* LFE */
		SNDRV_CHMAP_RL,		/* back left */
		SNDRV_CHMAP_RR,		/* back right */
		SNDRV_CHMAP_FLC,	/* front left of center */
		SNDRV_CHMAP_FRC,	/* front right of center */
		SNDRV_CHMAP_RC,		/* back center */
		SNDRV_CHMAP_SL,		/* side left */
		SNDRV_CHMAP_SR,		/* side right */
		SNDRV_CHMAP_TC,		/* top center */
		SNDRV_CHMAP_TFL,	/* top front left */
		SNDRV_CHMAP_TFC,	/* top front center */
		SNDRV_CHMAP_TFR,	/* top front right */
		SNDRV_CHMAP_TRL,	/* top back left */
		SNDRV_CHMAP_TRC,	/* top back center */
		SNDRV_CHMAP_TRR,	/* top back right */
		SNDRV_CHMAP_TFLC,	/* top front left of center */
		SNDRV_CHMAP_TFRC,	/* top front right of center */
		SNDRV_CHMAP_LLFE,	/* left LFE */
		SNDRV_CHMAP_RLFE,	/* right LFE */
		SNDRV_CHMAP_TSL,	/* top side left */
		SNDRV_CHMAP_TSR,	/* top side right */
		SNDRV_CHMAP_BC,		/* bottom center */
		SNDRV_CHMAP_RLC,	/* back left of center */
		SNDRV_CHMAP_RRC,	/* back right of center */
		0 /* terminator */
	};
	struct snd_pcm_chmap_elem *chmap;
	const unsigned int *maps;
	int c;

	if (channels > ARRAY_SIZE(chmap->map))
		return NULL;

	chmap = kzalloc(sizeof(*chmap), GFP_KERNEL);
	if (!chmap)
		return NULL;

	maps = protocol == UAC_VERSION_2 ? uac2_maps : uac1_maps;
	chmap->channels = channels;
	c = 0;

	if (bits) {
		for (; bits && *maps; maps++, bits >>= 1)
			if (bits & 1)
				chmap->map[c++] = *maps;
	} else {
		/* If we're missing wChannelConfig, then guess something
		    to make sure the channel map is not skipped entirely */
		if (channels == 1)
			chmap->map[c++] = SNDRV_CHMAP_MONO;
		else
			for (; c < channels && *maps; maps++)
				chmap->map[c++] = *maps;
	}

	for (; c < channels; c++)
		chmap->map[c] = SNDRV_CHMAP_UNKNOWN;

	return chmap;
}