addRuleName(FileInfo *nested, CharsString *name, TranslationTableOffset *newRuleOffset,
		RuleName **ruleNames, TranslationTableHeader *table) {
	int k;
	struct RuleName *nameRule;
	if (!(nameRule = malloc(sizeof(*nameRule) + CHARSIZE * (name->length - 1)))) {
		compileError(nested, "not enough memory");
		_lou_outOfMemory();
	}
	memset(nameRule, 0, sizeof(*nameRule));
	for (k = 0; k < name->length; k++) {
		TranslationTableCharacter *ch =
				definedCharOrDots(nested, name->chars[k], 0, table);
		if (!(ch->attributes & CTC_Letter)) {
			compileError(nested, "a name may contain only letters");
			return 0;
		}
		nameRule->name[k] = name->chars[k];
	}
	nameRule->length = name->length;
	nameRule->ruleOffset = *newRuleOffset;
	nameRule->next = *ruleNames;
	*ruleNames = nameRule;
	return 1;
}