cirrus_hook_read_sr(CirrusVGAState * s, unsigned reg_index, int *reg_value)
{
    switch (reg_index) {
    case 0x00:			// Standard VGA
    case 0x01:			// Standard VGA
    case 0x02:			// Standard VGA
    case 0x03:			// Standard VGA
    case 0x04:			// Standard VGA
	return CIRRUS_HOOK_NOT_HANDLED;
    case 0x06:			// Unlock Cirrus extensions
	*reg_value = s->sr[reg_index];
	break;
    case 0x10:
    case 0x30:
    case 0x50:
    case 0x70:			// Graphics Cursor X
    case 0x90:
    case 0xb0:
    case 0xd0:
    case 0xf0:			// Graphics Cursor X
	*reg_value = s->sr[0x10];
	break;
    case 0x11:
    case 0x31:
    case 0x51:
    case 0x71:			// Graphics Cursor Y
    case 0x91:
    case 0xb1:
    case 0xd1:
    case 0xf1:			// Graphics Cursor Y
	*reg_value = s->sr[0x11];
	break;
    case 0x05:			// ???
    case 0x07:			// Extended Sequencer Mode
    case 0x08:			// EEPROM Control
    case 0x09:			// Scratch Register 0
    case 0x0a:			// Scratch Register 1
    case 0x0b:			// VCLK 0
    case 0x0c:			// VCLK 1
    case 0x0d:			// VCLK 2
    case 0x0e:			// VCLK 3
    case 0x0f:			// DRAM Control
    case 0x12:			// Graphics Cursor Attribute
    case 0x13:			// Graphics Cursor Pattern Address
    case 0x14:			// Scratch Register 2
    case 0x15:			// Scratch Register 3
    case 0x16:			// Performance Tuning Register
    case 0x17:			// Configuration Readback and Extended Control
    case 0x18:			// Signature Generator Control
    case 0x19:			// Signal Generator Result
    case 0x1a:			// Signal Generator Result
    case 0x1b:			// VCLK 0 Denominator & Post
    case 0x1c:			// VCLK 1 Denominator & Post
    case 0x1d:			// VCLK 2 Denominator & Post
    case 0x1e:			// VCLK 3 Denominator & Post
    case 0x1f:			// BIOS Write Enable and MCLK select
#ifdef DEBUG_CIRRUS
	printf("cirrus: handled inport sr_index %02x\n", reg_index);
#endif
	*reg_value = s->sr[reg_index];
	break;
    default:
#ifdef DEBUG_CIRRUS
	printf("cirrus: inport sr_index %02x\n", reg_index);
#endif
	*reg_value = 0xff;
	break;
    }

    return CIRRUS_HOOK_HANDLED;
}