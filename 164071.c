htmlGetEndPriority (const xmlChar *name) {
    int i = 0;

    while ((htmlEndPriority[i].name != NULL) &&
	   (!xmlStrEqual((const xmlChar *)htmlEndPriority[i].name, name)))
	i++;

    return(htmlEndPriority[i].priority);
}