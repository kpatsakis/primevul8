static void amba_put_disable_pclk(struct amba_device *pcdev)
{
	clk_disable_unprepare(pcdev->pclk);
	clk_put(pcdev->pclk);
}