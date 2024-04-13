random_is_faked()
{
    if( !is_initialized )
	initialize();
    return faked_rng || quick_test;
}