PHP_FUNCTION(snmpget)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GET, SNMP_VERSION_1);
}