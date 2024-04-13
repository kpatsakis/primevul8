static int cirrus_hook_write_palette(CirrusVGAState * s, int reg_value)
{
    if (!(s->sr[0x12] & CIRRUS_CURSOR_HIDDENPEL))
	return CIRRUS_HOOK_NOT_HANDLED;
    s->dac_cache[s->dac_sub_index] = reg_value;
    if (++s->dac_sub_index == 3) {
        memcpy(&s->cirrus_hidden_palette[(s->dac_write_index & 0x0f) * 3],
               s->dac_cache, 3);
        /* XXX update cursor */
	s->dac_sub_index = 0;
	s->dac_write_index++;
    }
    return CIRRUS_HOOK_HANDLED;
}