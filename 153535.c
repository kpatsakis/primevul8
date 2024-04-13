same_url_p(ParsedURL *pu1, ParsedURL *pu2)
{
    return (pu1->scheme == pu2->scheme && pu1->port == pu2->port &&
	    (pu1->host ? pu2->host ? !strcasecmp(pu1->host, pu2->host) : 0 : 1)
	    && (pu1->file ? pu2->
		file ? !strcmp(pu1->file, pu2->file) : 0 : 1));
}