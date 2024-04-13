const struct resource_caps *dce110_resource_cap(
	struct hw_asic_id *asic_id)
{
	if (ASIC_REV_IS_STONEY(asic_id->hw_internal_rev))
		return &stoney_resource_cap;
	else
		return &carrizo_resource_cap;
}