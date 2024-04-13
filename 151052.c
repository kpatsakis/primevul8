containsNonAlphaNumDash(const LString &s) {
	const LString::Part *part = s.start;
	while (part != NULL) {
		for (unsigned int i = 0; i < part->size; i++) {
			const char start = part->data[i];
			if (start != '-' && !isAlphaNum(start)) {
				return true;
			}
		}
		part = part->next;
	}
	return false;
}