static void cirrus_read_hidden_dac(CirrusVGAState * s, int *reg_value)
{
    *reg_value = 0xff;
    if (++s->cirrus_hidden_dac_lockindex == 5) {
        *reg_value = s->cirrus_hidden_dac_data;
	s->cirrus_hidden_dac_lockindex = 0;
    }
}