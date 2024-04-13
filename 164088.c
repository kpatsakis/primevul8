htmlEntityValueLookup(unsigned int value) {
    unsigned int i;

    for (i = 0;i < (sizeof(html40EntitiesTable)/
                    sizeof(html40EntitiesTable[0]));i++) {
        if (html40EntitiesTable[i].value >= value) {
	    if (html40EntitiesTable[i].value > value)
		break;
            return((htmlEntityDescPtr) &html40EntitiesTable[i]);
	}
    }
    return(NULL);
}