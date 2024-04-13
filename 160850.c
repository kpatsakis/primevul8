read_seed_file(void)
{
    int fd;
    struct stat sb;
    unsigned char buffer[POOLSIZE];
    int n;

    if( !seed_file_name )
	return 0;

#if defined(HAVE_DOSISH_SYSTEM) || defined(__CYGWIN__)
    fd = open( seed_file_name, O_RDONLY | O_BINARY );
#elif defined( __VMS)
    /* We're only reading, so allow others to do anything. */
    fd = open( seed_file_name, O_RDONLY, 0777, "shr=get,put,upd" );
#else
    fd = open( seed_file_name, O_RDONLY );
#endif
    if( fd == -1 && errno == ENOENT) {
	allow_seed_file_update = 1;
	return 0;
    }

    if( fd == -1 ) {
	log_info(_("can't open `%s': %s\n"), seed_file_name, strerror(errno) );
	return 0;
    }
    if (lock_seed_file (fd, seed_file_name, 0))
      {
        close (fd);
        return 0;
      }

    if( fstat( fd, &sb ) ) {
	log_info(_("can't stat `%s': %s\n"), seed_file_name, strerror(errno) );
	close(fd);
	return 0;
    }
    if( !S_ISREG(sb.st_mode) ) {
	log_info(_("`%s' is not a regular file - ignored\n"), seed_file_name );
	close(fd);
	return 0;
    }
    if( !sb.st_size ) {
	log_info(_("note: random_seed file is empty\n") );
	close(fd);
	allow_seed_file_update = 1;
	return 0;
    }
    if( sb.st_size != POOLSIZE ) {
	log_info(_("WARNING: invalid size of random_seed file - not used\n") );
	close(fd);
	return 0;
    }

    do {
	n = read( fd, buffer, POOLSIZE );
    } while( n == -1 && errno == EINTR );
    /* The N==0, ENOENT, and N!=POOLSIZE cases may happen if another
       process is updating the file.  For consistency we use the same
       recovery strategy as with the pre-read checks.  */
    if (!n) {
        log_info(_("note: random_seed file is empty\n") );
        allow_seed_file_update = 1;
        close(fd);
        return 0;
    }
    else if( n == -1 && errno == ENOENT) {
        /* On a Unix system that should never happen.  However, I can
           imagine this error code on non-inode based systems.  */
	log_info(_("can't read `%s': %s\n"), seed_file_name, strerror(errno));
        allow_seed_file_update = 1;
	close(fd);
	return 0;
    }
    else if( n == -1 ) {
        /* A real read error.  */
	log_fatal(_("can't read `%s': %s\n"), seed_file_name,strerror(errno) );
	close(fd);
	return 0;
    }
    else if ( n != POOLSIZE ) {
        log_info(_("WARNING: invalid size of random_seed file - not used\n") );
	close(fd);
	return 0;
    }

    close(fd);

    add_randomness( buffer, POOLSIZE, 0 );
    /* add some minor entropy to the pool now (this will also force a mixing) */
    {	pid_t x = getpid();
	add_randomness( &x, sizeof(x), 0 );
    }
    {	time_t x = time(NULL);
	add_randomness( &x, sizeof(x), 0 );
    }
    {	clock_t x = clock();
	add_randomness( &x, sizeof(x), 0 );
    }
    /* And read a few bytes from our entropy source.  By using
     * a level of 0 this will not block and might not return anything
     * with some entropy drivers, however the rndlinux driver will use
     * /dev/urandom and return some stuff - Do not read too much as we
     * want to be friendly to the scarce system entropy resource. */
    read_random_source( 0, 16, 0 );

    allow_seed_file_update = 1;
    return 1;
}