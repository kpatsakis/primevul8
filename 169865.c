addRule(FileInfo *nested, TranslationTableOpcode opcode, CharsString *ruleChars,
		CharsString *ruleDots, TranslationTableCharacterAttributes after,
		TranslationTableCharacterAttributes before, TranslationTableOffset *newRuleOffset,
		TranslationTableRule **newRule, int noback, int nofor,
		TranslationTableHeader **table) {
	/* Add a rule to the table, using the hash function to find the start of
	 * chains and chaining both the chars and dots strings */
	int ruleSize = sizeof(TranslationTableRule) - (DEFAULTRULESIZE * CHARSIZE);
	if (ruleChars) ruleSize += CHARSIZE * ruleChars->length;
	if (ruleDots) ruleSize += CHARSIZE * ruleDots->length;
	if (!allocateSpaceInTable(nested, newRuleOffset, ruleSize, table)) return 0;
	TranslationTableRule *rule =
			(TranslationTableRule *)&(*table)->ruleArea[*newRuleOffset];
	*newRule = rule;
	rule->opcode = opcode;
	rule->after = after;
	rule->before = before;
	if (ruleChars)
		memcpy(&rule->charsdots[0], &ruleChars->chars[0],
				CHARSIZE * (rule->charslen = ruleChars->length));
	else
		rule->charslen = 0;
	if (ruleDots)
		memcpy(&rule->charsdots[rule->charslen], &ruleDots->chars[0],
				CHARSIZE * (rule->dotslen = ruleDots->length));
	else
		rule->dotslen = 0;
	if (!charactersDefined(nested, rule, *table)) return 0;

	/* link new rule into table. */
	if (opcode == CTO_SwapCc || opcode == CTO_SwapCd || opcode == CTO_SwapDd) return 1;
	if (opcode >= CTO_Context && opcode <= CTO_Pass4)
		if (!(opcode == CTO_Context && rule->charslen > 0)) {
			if (!nofor)
				if (!addForwardPassRule(newRuleOffset, rule, *table)) return 0;
			if (!noback)
				if (!addBackwardPassRule(newRuleOffset, rule, *table)) return 0;
			return 1;
		}
	if (!nofor) {
		if (rule->charslen == 1)
			addForwardRuleWithSingleChar(nested, newRuleOffset, rule, *table);
		else if (rule->charslen > 1)
			addForwardRuleWithMultipleChars(newRuleOffset, rule, *table);
	}
	if (!noback) {
		widechar *cells;
		int count;

		if (rule->opcode == CTO_Context) {
			cells = &rule->charsdots[0];
			count = rule->charslen;
		} else {
			cells = &rule->charsdots[rule->charslen];
			count = rule->dotslen;
		}

		if (count == 1)
			addBackwardRuleWithSingleCell(nested, *cells, newRuleOffset, rule, *table);
		else if (count > 1)
			addBackwardRuleWithMultipleCells(cells, count, newRuleOffset, rule, *table);
	}
	return 1;
}