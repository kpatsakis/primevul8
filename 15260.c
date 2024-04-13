sonmp_suite(void)
{
	Suite *s = suite_create("SONMP");

#ifdef ENABLE_SONMP
	TCase *tc_send = tcase_create("Send SONMP packets");
	TCase *tc_receive = tcase_create("Receive SONMP packets");

	tcase_add_checked_fixture(tc_send, pcap_setup, pcap_teardown);
	tcase_add_test(tc_send, test_send_sonmp);
	suite_add_tcase(s, tc_send);

	tcase_add_test(tc_receive, test_recv_sonmp);
	suite_add_tcase(s, tc_receive);
#endif

	return s;
}