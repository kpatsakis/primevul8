static int parse_options(char ** optionsp, unsigned long * filesys_flags)
{
	const char * data;
	char * percent_char = NULL;
	char * value = NULL;
	char * next_keyword = NULL;
	char * out = NULL;
	int out_len = 0;
	int word_len;
	int rc = 0;
	char user[32];
	char group[32];

	if (!optionsp || !*optionsp)
		return 1;
	data = *optionsp;

	/* BB fixme check for separator override BB */

	if (getuid()) {
		got_uid = 1;
		snprintf(user,sizeof(user),"%u",getuid());
		got_gid = 1;
		snprintf(group,sizeof(group),"%u",getgid());
	}

/* while ((data = strsep(&options, ",")) != NULL) { */
	while(data != NULL) {
		/*  check if ends with trailing comma */
		if(*data == 0)
			break;

		/* format is keyword=value,keyword2=value2,keyword3=value3 etc.) */
		/* data  = next keyword */
		/* value = next value ie stuff after equal sign */

		next_keyword = strchr(data,','); /* BB handle sep= */
	
		/* temporarily null terminate end of keyword=value pair */
		if(next_keyword)
			*next_keyword++ = 0;

		/* temporarily null terminate keyword to make keyword and value distinct */
		if ((value = strchr(data, '=')) != NULL) {
			*value = '\0';
			value++;
		}

		if (strncmp(data, "users",5) == 0) {
			if(!value || !*value) {
				*filesys_flags |= MS_USERS;
				goto nocopy;
			}
		} else if (strncmp(data, "user_xattr",10) == 0) {
		   /* do nothing - need to skip so not parsed as user name */
		} else if (strncmp(data, "user", 4) == 0) {

			if (!value || !*value) {
				if(data[4] == '\0') {
					*filesys_flags |= MS_USER;
					goto nocopy;
				} else {
					fprintf(stderr, "username specified with no parameter\n");
					SAFE_FREE(out);
					return 1;	/* needs_arg; */
				}
			} else {
				if (strnlen(value, 260) < 260) {
					got_user=1;
					percent_char = strchr(value,'%');
					if(percent_char) {
						*percent_char = ',';
						if(mountpassword == NULL)
							mountpassword = (char *)calloc(MOUNT_PASSWD_SIZE+1,1);
						if(mountpassword) {
							if(got_password)
								fprintf(stderr, "\nmount.cifs warning - password specified twice\n");
							got_password = 1;
							percent_char++;
							strlcpy(mountpassword, percent_char,MOUNT_PASSWD_SIZE+1);
						/*  remove password from username */
							while(*percent_char != 0) {
								*percent_char = ',';
								percent_char++;
							}
						}
					}
					/* this is only case in which the user
					name buf is not malloc - so we have to
					check for domain name embedded within
					the user name here since the later
					call to check_for_domain will not be
					invoked */
					domain_name = check_for_domain(&value);
				} else {
					fprintf(stderr, "username too long\n");
					SAFE_FREE(out);
					return 1;
				}
			}
		} else if (strncmp(data, "pass", 4) == 0) {
			if (!value || !*value) {
				if(got_password) {
					fprintf(stderr, "\npassword specified twice, ignoring second\n");
				} else
					got_password = 1;
			} else if (strnlen(value, MOUNT_PASSWD_SIZE) < MOUNT_PASSWD_SIZE) {
				if (got_password) {
					fprintf(stderr, "\nmount.cifs warning - password specified twice\n");
				} else {
					mountpassword = strndup(value, MOUNT_PASSWD_SIZE);
					if (!mountpassword) {
						fprintf(stderr, "mount.cifs error: %s", strerror(ENOMEM));
						SAFE_FREE(out);
						return 1;
					}
					got_password = 1;
				}
			} else {
				fprintf(stderr, "password too long\n");
				SAFE_FREE(out);
				return 1;
			}
			goto nocopy;
		} else if (strncmp(data, "sec", 3) == 0) {
			if (value) {
				if (!strncmp(value, "none", 4) ||
				    !strncmp(value, "krb5", 4))
					got_password = 1;
			}
		} else if (strncmp(data, "ip", 2) == 0) {
			if (!value || !*value) {
				fprintf(stderr, "target ip address argument missing");
			} else if (strnlen(value, MAX_ADDRESS_LEN) <= MAX_ADDRESS_LEN) {
				if(verboseflag)
					fprintf(stderr, "ip address %s override specified\n",value);
				got_ip = 1;
			} else {
				fprintf(stderr, "ip address too long\n");
				SAFE_FREE(out);
				return 1;
			}
		} else if ((strncmp(data, "unc", 3) == 0)
		   || (strncmp(data, "target", 6) == 0)
		   || (strncmp(data, "path", 4) == 0)) {
			if (!value || !*value) {
				fprintf(stderr, "invalid path to network resource\n");
				SAFE_FREE(out);
				return 1;  /* needs_arg; */
			} else if(strnlen(value,5) < 5) {
				fprintf(stderr, "UNC name too short");
			}

			if (strnlen(value, 300) < 300) {
				got_unc = 1;
				if (strncmp(value, "//", 2) == 0) {
					if(got_unc)
						fprintf(stderr, "unc name specified twice, ignoring second\n");
					else
						got_unc = 1;
				} else if (strncmp(value, "\\\\", 2) != 0) {	                   
					fprintf(stderr, "UNC Path does not begin with // or \\\\ \n");
					SAFE_FREE(out);
					return 1;
				} else {
					if(got_unc)
						fprintf(stderr, "unc name specified twice, ignoring second\n");
					else
						got_unc = 1;
				}
			} else {
				fprintf(stderr, "CIFS: UNC name too long\n");
				SAFE_FREE(out);
				return 1;
			}
		} else if ((strncmp(data, "dom" /* domain */, 3) == 0)
			   || (strncmp(data, "workg", 5) == 0)) {
			/* note this allows for synonyms of "domain"
			   such as "DOM" and "dom" and "workgroup"
			   and "WORKGRP" etc. */
			if (!value || !*value) {
				fprintf(stderr, "CIFS: invalid domain name\n");
				SAFE_FREE(out);
				return 1;	/* needs_arg; */
			}
			if (strnlen(value, DOMAIN_SIZE+1) < DOMAIN_SIZE+1) {
				got_domain = 1;
			} else {
				fprintf(stderr, "domain name too long\n");
				SAFE_FREE(out);
				return 1;
			}
		} else if (strncmp(data, "cred", 4) == 0) {
			if (value && *value) {
				rc = open_cred_file(value);
				if(rc) {
					fprintf(stderr, "error %d (%s) opening credential file %s\n",
						rc, strerror(rc), value);
					SAFE_FREE(out);
					return 1;
				}
			} else {
				fprintf(stderr, "invalid credential file name specified\n");
				SAFE_FREE(out);
				return 1;
			}
		} else if (strncmp(data, "uid", 3) == 0) {
			if (value && *value) {
				got_uid = 1;
				if (!isdigit(*value)) {
					struct passwd *pw;

					if (!(pw = getpwnam(value))) {
						fprintf(stderr, "bad user name \"%s\"\n", value);
						exit(EX_USAGE);
					}
					snprintf(user, sizeof(user), "%u", pw->pw_uid);
				} else {
					strlcpy(user,value,sizeof(user));
				}
			}
			goto nocopy;
		} else if (strncmp(data, "gid", 3) == 0) {
			if (value && *value) {
				got_gid = 1;
				if (!isdigit(*value)) {
					struct group *gr;

					if (!(gr = getgrnam(value))) {
						fprintf(stderr, "bad group name \"%s\"\n", value);
						exit(EX_USAGE);
					}
					snprintf(group, sizeof(group), "%u", gr->gr_gid);
				} else {
					strlcpy(group,value,sizeof(group));
				}
			}
			goto nocopy;
       /* fmask and dmask synonyms for people used to smbfs syntax */
		} else if (strcmp(data, "file_mode") == 0 || strcmp(data, "fmask")==0) {
			if (!value || !*value) {
				fprintf(stderr, "Option '%s' requires a numerical argument\n", data);
				SAFE_FREE(out);
				return 1;
			}

			if (value[0] != '0') {
				fprintf(stderr, "WARNING: '%s' not expressed in octal.\n", data);
			}

			if (strcmp (data, "fmask") == 0) {
				fprintf(stderr, "WARNING: CIFS mount option 'fmask' is deprecated. Use 'file_mode' instead.\n");
				data = "file_mode"; /* BB fix this */
			}
		} else if (strcmp(data, "dir_mode") == 0 || strcmp(data, "dmask")==0) {
			if (!value || !*value) {
				fprintf(stderr, "Option '%s' requires a numerical argument\n", data);
				SAFE_FREE(out);
				return 1;
			}

			if (value[0] != '0') {
				fprintf(stderr, "WARNING: '%s' not expressed in octal.\n", data);
			}

			if (strcmp (data, "dmask") == 0) {
				fprintf(stderr, "WARNING: CIFS mount option 'dmask' is deprecated. Use 'dir_mode' instead.\n");
				data = "dir_mode";
			}
			/* the following eight mount options should be
			stripped out from what is passed into the kernel
			since these eight options are best passed as the
			mount flags rather than redundantly to the kernel 
			and could generate spurious warnings depending on the
			level of the corresponding cifs vfs kernel code */
		} else if (strncmp(data, "nosuid", 6) == 0) {
			*filesys_flags |= MS_NOSUID;
		} else if (strncmp(data, "suid", 4) == 0) {
			*filesys_flags &= ~MS_NOSUID;
		} else if (strncmp(data, "nodev", 5) == 0) {
			*filesys_flags |= MS_NODEV;
		} else if ((strncmp(data, "nobrl", 5) == 0) || 
			   (strncmp(data, "nolock", 6) == 0)) {
			*filesys_flags &= ~MS_MANDLOCK;
		} else if (strncmp(data, "dev", 3) == 0) {
			*filesys_flags &= ~MS_NODEV;
		} else if (strncmp(data, "noexec", 6) == 0) {
			*filesys_flags |= MS_NOEXEC;
		} else if (strncmp(data, "exec", 4) == 0) {
			*filesys_flags &= ~MS_NOEXEC;
		} else if (strncmp(data, "guest", 5) == 0) {
			user_name = (char *)calloc(1, 1);
			got_user = 1;
			got_password = 1;
		} else if (strncmp(data, "ro", 2) == 0) {
			*filesys_flags |= MS_RDONLY;
			goto nocopy;
		} else if (strncmp(data, "rw", 2) == 0) {
			*filesys_flags &= ~MS_RDONLY;
			goto nocopy;
                } else if (strncmp(data, "remount", 7) == 0) {
                        *filesys_flags |= MS_REMOUNT;
		} /* else if (strnicmp(data, "port", 4) == 0) {
			if (value && *value) {
				vol->port =
					simple_strtoul(value, &value, 0);
			}
		} else if (strnicmp(data, "rsize", 5) == 0) {
			if (value && *value) {
				vol->rsize =
					simple_strtoul(value, &value, 0);
			}
		} else if (strnicmp(data, "wsize", 5) == 0) {
			if (value && *value) {
				vol->wsize =
					simple_strtoul(value, &value, 0);
			}
		} else if (strnicmp(data, "version", 3) == 0) {
		} else {
			fprintf(stderr, "CIFS: Unknown mount option %s\n",data);
		} */ /* nothing to do on those four mount options above.
			Just pass to kernel and ignore them here */

		/* Copy (possibly modified) option to out */
		word_len = strlen(data);
		if (value)
			word_len += 1 + strlen(value);

		out = (char *)realloc(out, out_len + word_len + 2);
		if (out == NULL) {
			perror("malloc");
			exit(EX_SYSERR);
		}

		if (out_len) {
			strlcat(out, ",", out_len + word_len + 2);
			out_len++;
		}

		if (value)
			snprintf(out + out_len, word_len + 1, "%s=%s", data, value);
		else
			snprintf(out + out_len, word_len + 1, "%s", data);
		out_len = strlen(out);

nocopy:
		data = next_keyword;
	}

	/* special-case the uid and gid */
	if (got_uid) {
		word_len = strlen(user);

		out = (char *)realloc(out, out_len + word_len + 6);
		if (out == NULL) {
			perror("malloc");
			exit(EX_SYSERR);
		}

		if (out_len) {
			strlcat(out, ",", out_len + word_len + 6);
			out_len++;
		}
		snprintf(out + out_len, word_len + 5, "uid=%s", user);
		out_len = strlen(out);
	}
	if (got_gid) {
		word_len = strlen(group);

		out = (char *)realloc(out, out_len + 1 + word_len + 6);
		if (out == NULL) {
		perror("malloc");
			exit(EX_SYSERR);
		}

		if (out_len) {
			strlcat(out, ",", out_len + word_len + 6);
			out_len++;
		}
		snprintf(out + out_len, word_len + 5, "gid=%s", group);
		out_len = strlen(out);
	}

	SAFE_FREE(*optionsp);
	*optionsp = out;
	return 0;
}