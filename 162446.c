PHP_FUNCTION(snmp3_real_walk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_WALK, SNMP_VERSION_3);
}