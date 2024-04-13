get_auth_param(struct auth_param *auth, char *name)
{
    struct auth_param *ap;
    for (ap = auth; ap->name != NULL; ap++) {
	if (strcasecmp(name, ap->name) == 0)
	    return ap->val;
    }
    return NULL;
}