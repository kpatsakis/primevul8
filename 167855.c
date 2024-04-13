static int get_password_from_file(int file_descript, char * filename)
{
	int rc = 0;
	int i;
	char c;

	if(mountpassword == NULL)
		mountpassword = (char *)calloc(MOUNT_PASSWD_SIZE+1,1);
	else 
		memset(mountpassword, 0, MOUNT_PASSWD_SIZE);

	if (mountpassword == NULL) {
		fprintf(stderr, "malloc failed\n");
		exit(EX_SYSERR);
	}

	if(filename != NULL) {
		rc = access(filename, R_OK);
		if (rc) {
			fprintf(stderr, "mount.cifs failed: access check of %s failed: %s\n",
					filename, strerror(errno));
			exit(EX_SYSERR);
		}
		file_descript = open(filename, O_RDONLY);
		if(file_descript < 0) {
			fprintf(stderr, "mount.cifs failed. %s attempting to open password file %s\n",
				   strerror(errno),filename);
			exit(EX_SYSERR);
		}
	}
	/* else file already open and fd provided */

	for(i=0;i<MOUNT_PASSWD_SIZE;i++) {
		rc = read(file_descript,&c,1);
		if(rc < 0) {
			fprintf(stderr, "mount.cifs failed. Error %s reading password file\n",strerror(errno));
			if(filename != NULL)
				close(file_descript);
			exit(EX_SYSERR);
		} else if(rc == 0) {
			if(mountpassword[0] == 0) {
				if(verboseflag)
					fprintf(stderr, "\nWarning: null password used since cifs password file empty");
			}
			break;
		} else /* read valid character */ {
			if((c == 0) || (c == '\n')) {
				mountpassword[i] = '\0';
				break;
			} else 
				mountpassword[i] = c;
		}
	}
	if((i == MOUNT_PASSWD_SIZE) && (verboseflag)) {
		fprintf(stderr, "\nWarning: password longer than %d characters specified in cifs password file",
			MOUNT_PASSWD_SIZE);
	}
	got_password = 1;
	if(filename != NULL) {
		close(file_descript);
	}

	return rc;
}