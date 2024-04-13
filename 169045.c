static void k_brlcommit(struct vc_data *vc, unsigned int pattern, char up_flag)
{
	static unsigned long chords;
	static unsigned committed;

	if (!brl_nbchords)
		k_deadunicode(vc, BRL_UC_ROW | pattern, up_flag);
	else {
		committed |= pattern;
		chords++;
		if (chords == brl_nbchords) {
			k_unicode(vc, BRL_UC_ROW | committed, up_flag);
			chords = 0;
			committed = 0;
		}
	}
}