readin_config(char *fname)
{
	if ((yyin = fopen(fname, "r")) == NULL)
	{
		flog(LOG_ERR, "can't open %s: %s", fname, strerror(errno));
		return (-1);
	}

	if (yyparse() != 0)
	{
		flog(LOG_ERR, "error parsing or activating the config file: %s", fname);
		return (-1);
	}

	fclose(yyin);
	return 0;
}