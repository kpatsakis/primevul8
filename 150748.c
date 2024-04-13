cirrus_hook_read_gr(CirrusVGAState * s, unsigned reg_index, int *reg_value)
{
    switch (reg_index) {
    case 0x00: // Standard VGA, BGCOLOR 0x000000ff
      *reg_value = s->cirrus_shadow_gr0;
      return CIRRUS_HOOK_HANDLED;
    case 0x01: // Standard VGA, FGCOLOR 0x000000ff
      *reg_value = s->cirrus_shadow_gr1;
      return CIRRUS_HOOK_HANDLED;
    case 0x02:			// Standard VGA
    case 0x03:			// Standard VGA
    case 0x04:			// Standard VGA
    case 0x06:			// Standard VGA
    case 0x07:			// Standard VGA
    case 0x08:			// Standard VGA
	return CIRRUS_HOOK_NOT_HANDLED;
    case 0x05:			// Standard VGA, Cirrus extended mode
    default:
	break;
    }

    if (reg_index < 0x3a) {
	*reg_value = s->gr[reg_index];
    } else {
#ifdef DEBUG_CIRRUS
	printf("cirrus: inport gr_index %02x\n", reg_index);
#endif
	*reg_value = 0xff;
    }

    return CIRRUS_HOOK_HANDLED;
}