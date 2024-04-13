static int open_cred_file(char * file_name)
{
	char * line_buf;
	char * temp_val;
	FILE * fs;
	int i, length;

	i = access(file_name, R_OK);
	if (i)
		return i;

	fs = fopen(file_name,"r");
	if(fs == NULL)
		return errno;
	line_buf = (char *)malloc(4096);
	if(line_buf == NULL) {
		fclose(fs);
		return ENOMEM;
	}

	while(fgets(line_buf,4096,fs)) {
		/* parse line from credential file */

		/* eat leading white space */
		for(i=0;i<4086;i++) {
			if((line_buf[i] != ' ') && (line_buf[i] != '\t'))
				break;
			/* if whitespace - skip past it */
		}
		if (strncasecmp("username",line_buf+i,8) == 0) {
			temp_val = strchr(line_buf + i,'=');
			if(temp_val) {
				/* go past equals sign */
				temp_val++;
				for(length = 0;length<4087;length++) {
					if ((temp_val[length] == '\n')
					    || (temp_val[length] == '\0')) {
						temp_val[length] = '\0';
						break;
					}
				}
				if(length > 4086) {
					fprintf(stderr, "mount.cifs failed due to malformed username in credentials file\n");
					memset(line_buf,0,4096);
					exit(EX_USAGE);
				} else {
					got_user = 1;
					user_name = (char *)calloc(1 + length,1);
					/* BB adding free of user_name string before exit,
						not really necessary but would be cleaner */
					strlcpy(user_name,temp_val, length+1);
				}
			}
		} else if (strncasecmp("password",line_buf+i,8) == 0) {
			temp_val = strchr(line_buf+i,'=');
			if(temp_val) {
				/* go past equals sign */
				temp_val++;
				for(length = 0;length<MOUNT_PASSWD_SIZE+1;length++) {
					if ((temp_val[length] == '\n')
					    || (temp_val[length] == '\0')) {
						temp_val[length] = '\0';
						break;
					}
				}
				if(length > MOUNT_PASSWD_SIZE) {
					fprintf(stderr, "mount.cifs failed: password in credentials file too long\n");
					memset(line_buf,0, 4096);
					exit(EX_USAGE);
				} else {
					if(mountpassword == NULL) {
						mountpassword = (char *)calloc(MOUNT_PASSWD_SIZE+1,1);
					} else
						memset(mountpassword,0,MOUNT_PASSWD_SIZE);
					if(mountpassword) {
						strlcpy(mountpassword,temp_val,MOUNT_PASSWD_SIZE+1);
						got_password = 1;
					}
				}
			}
                } else if (strncasecmp("domain",line_buf+i,6) == 0) {
                        temp_val = strchr(line_buf+i,'=');
                        if(temp_val) {
                                /* go past equals sign */
                                temp_val++;
				if(verboseflag)
					fprintf(stderr, "\nDomain %s\n",temp_val);
                                for(length = 0;length<DOMAIN_SIZE+1;length++) {
					if ((temp_val[length] == '\n')
					    || (temp_val[length] == '\0')) {
						temp_val[length] = '\0';
						break;
					}
                                }
                                if(length > DOMAIN_SIZE) {
                                        fprintf(stderr, "mount.cifs failed: domain in credentials file too long\n");
                                        exit(EX_USAGE);
                                } else {
                                        if(domain_name == NULL) {
                                                domain_name = (char *)calloc(DOMAIN_SIZE+1,1);
                                        } else
                                                memset(domain_name,0,DOMAIN_SIZE);
                                        if(domain_name) {
                                                strlcpy(domain_name,temp_val,DOMAIN_SIZE+1);
                                                got_domain = 1;
                                        }
                                }
                        }
                }

	}
	fclose(fs);
	SAFE_FREE(line_buf);
	return 0;
}