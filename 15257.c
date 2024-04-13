main()
{
	int number_failed;
	Suite *s = sonmp_suite ();
	SRunner *sr = srunner_create (s);
	srunner_set_fork_status (sr, CK_NOFORK); /* Can't fork because
						    we need to write
						    files */
	srunner_run_all (sr, CK_ENV);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}