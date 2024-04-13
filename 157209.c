const struct resource_caps *dce112_resource_cap(
	struct hw_asic_id *asic_id)
{
	if (ASIC_REV_IS_POLARIS11_M(asic_id->hw_internal_rev) ||
	    ASIC_REV_IS_POLARIS12_V(asic_id->hw_internal_rev))
		return &polaris_11_resource_cap;
	else
		return &polaris_10_resource_cap;
}