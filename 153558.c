is_blank_line(char *line, int indent)
{
    int i, is_blank = 0;

    for (i = 0; i < indent; i++) {
	if (line[i] == '\0') {
	    is_blank = 1;
	}
	else if (line[i] != ' ') {
	    break;
	}
    }
    if (i == indent && line[i] == '\0')
	is_blank = 1;
    return is_blank;
}