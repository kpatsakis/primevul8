deallocateRuleNames(RuleName **ruleNames) {
	while (*ruleNames) {
		struct RuleName *nameRule = *ruleNames;
		*ruleNames = nameRule->next;
		if (nameRule) free(nameRule);
	}
}