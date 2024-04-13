PHP_MINFO_FUNCTION(snmp)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "NET-SNMP Support", "enabled");
	php_info_print_table_row(2, "NET-SNMP Version", netsnmp_get_version());
	php_info_print_table_row(2, "PHP SNMP Version", PHP_SNMP_VERSION);
	php_info_print_table_end();
}