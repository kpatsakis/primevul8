parse_server(char ** punc_name)
{
	char * unc_name = *punc_name;
	int length = strnlen(unc_name, MAX_UNC_LEN);
	char * share;
	struct addrinfo *addrlist;
	int rc;

	if(length > (MAX_UNC_LEN - 1)) {
		fprintf(stderr, "mount error: UNC name too long");
		return NULL;
	}
	if ((strncasecmp("cifs://", unc_name, 7) == 0) ||
	    (strncasecmp("smb://", unc_name, 6) == 0)) {
		fprintf(stderr, "\nMounting cifs URL not implemented yet. Attempt to mount %s\n", unc_name);
		return NULL;
	}

	if(length < 3) {
		/* BB add code to find DFS root here */
		fprintf(stderr, "\nMounting the DFS root for domain not implemented yet\n");
		return NULL;
	} else {
		if(strncmp(unc_name,"//",2) && strncmp(unc_name,"\\\\",2)) {
			/* check for nfs syntax ie server:share */
			share = strchr(unc_name,':');
			if(share) {
				*punc_name = (char *)malloc(length+3);
				if(*punc_name == NULL) {
					/* put the original string back  if 
					   no memory left */
					*punc_name = unc_name;
					return NULL;
				}
				*share = '/';
				strlcpy((*punc_name)+2,unc_name,length+1);
				SAFE_FREE(unc_name);
				unc_name = *punc_name;
				unc_name[length+2] = 0;
				goto continue_unc_parsing;
			} else {
				fprintf(stderr, "mount error: improperly formatted UNC name.");
				fprintf(stderr, " %s does not begin with \\\\ or //\n",unc_name);
				return NULL;
			}
		} else {
continue_unc_parsing:
			unc_name[0] = '/';
			unc_name[1] = '/';
			unc_name += 2;

			/* allow for either delimiter between host and sharename */
			if ((share = strpbrk(unc_name, "/\\"))) {
				*share = 0;  /* temporarily terminate the string */
				share += 1;
				if(got_ip == 0) {
					rc = getaddrinfo(unc_name, NULL, NULL, &addrlist);
					if (rc != 0) {
						fprintf(stderr, "mount error: could not resolve address for %s: %s\n",
							unc_name, gai_strerror(rc));
						addrlist = NULL;
					}
				}
				*(share - 1) = '/'; /* put delimiter back */

				/* we don't convert the prefixpath delimiters since '\\' is a valid char in posix paths */
				if ((prefixpath = strpbrk(share, "/\\"))) {
					*prefixpath = 0;  /* permanently terminate the string */
					if (!strlen(++prefixpath))
						prefixpath = NULL; /* this needs to be done explicitly */
				}
				if(got_ip) {
					if(verboseflag)
						fprintf(stderr, "ip address specified explicitly\n");
					return NULL;
				}
				/* BB should we pass an alternate version of the share name as Unicode */

				return addrlist; 
			} else {
				/* BB add code to find DFS root (send null path on get DFS Referral to specified server here */
				fprintf(stderr, "Mounting the DFS root for a particular server not implemented yet\n");
				return NULL;
			}
		}
	}
}