atTokenDelimiter(FileInfo *nested) {
	return nested->line[nested->linepos] <= 32;
}