set_random_seed_file( const char *name )
{
    if( seed_file_name )
	BUG();
    seed_file_name = xstrdup( name );
}