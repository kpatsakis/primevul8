PHP_METHOD(snmp, walk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_WALK, (-1));
}