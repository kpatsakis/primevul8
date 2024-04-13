htmlInitAutoClose(void) {
    int indx, i = 0;

    if (htmlStartCloseIndexinitialized) return;

    for (indx = 0;indx < 100;indx ++) htmlStartCloseIndex[indx] = NULL;
    indx = 0;
    while ((htmlStartClose[i] != NULL) && (indx < 100 - 1)) {
        htmlStartCloseIndex[indx++] = (const char**) &htmlStartClose[i];
	while (htmlStartClose[i] != NULL) i++;
	i++;
    }
    htmlStartCloseIndexinitialized = 1;
}