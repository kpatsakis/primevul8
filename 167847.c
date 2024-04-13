static void check_for_comma(char ** ppasswrd)
{
	char *new_pass_buf;
	char *pass;
	int i,j;
	int number_of_commas = 0;
	int len;

	if(ppasswrd == NULL)
		return;
	else 
		(pass = *ppasswrd);

	len = strlen(pass);

	for(i=0;i<len;i++)  {
		if(pass[i] == ',')
			number_of_commas++;
	}

	if(number_of_commas == 0)
		return;
	if(number_of_commas > MOUNT_PASSWD_SIZE) {
		/* would otherwise overflow the mount options buffer */
		fprintf(stderr, "\nInvalid password. Password contains too many commas.\n");
		return;
	}

	new_pass_buf = (char *)malloc(len+number_of_commas+1);
	if(new_pass_buf == NULL)
		return;

	for(i=0,j=0;i<len;i++,j++) {
		new_pass_buf[j] = pass[i];
		if(pass[i] == ',') {
			j++;
			new_pass_buf[j] = pass[i];
		}
	}
	new_pass_buf[len+number_of_commas] = 0;

	SAFE_FREE(*ppasswrd);
	*ppasswrd = new_pass_buf;
	
	return;
}