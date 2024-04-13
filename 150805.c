static void cirrus_update_bank_ptr(CirrusVGAState * s, unsigned bank_index)
{
    unsigned offset;
    unsigned limit;

    if ((s->gr[0x0b] & 0x01) != 0)	/* dual bank */
	offset = s->gr[0x09 + bank_index];
    else			/* single bank */
	offset = s->gr[0x09];

    if ((s->gr[0x0b] & 0x20) != 0)
	offset <<= 14;
    else
	offset <<= 12;

    if (s->real_vram_size <= offset)
	limit = 0;
    else
	limit = s->real_vram_size - offset;

    if (((s->gr[0x0b] & 0x01) == 0) && (bank_index != 0)) {
	if (limit > 0x8000) {
	    offset += 0x8000;
	    limit -= 0x8000;
	} else {
	    limit = 0;
	}
    }

    if (limit > 0) {
	s->cirrus_bank_base[bank_index] = offset;
	s->cirrus_bank_limit[bank_index] = limit;
    } else {
	s->cirrus_bank_base[bank_index] = 0;
	s->cirrus_bank_limit[bank_index] = 0;
    }
}