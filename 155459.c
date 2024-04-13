static int amba_get_enable_pclk(struct amba_device *pcdev)
{
	int ret;

	pcdev->pclk = clk_get(&pcdev->dev, "apb_pclk");
	if (IS_ERR(pcdev->pclk))
		return PTR_ERR(pcdev->pclk);

	ret = clk_prepare_enable(pcdev->pclk);
	if (ret)
		clk_put(pcdev->pclk);

	return ret;
}