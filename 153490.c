void iwl_pcie_ctxt_info_gen3_free(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	if (!trans_pcie->ctxt_info_gen3)
		return;

	dma_free_coherent(trans->dev, sizeof(*trans_pcie->ctxt_info_gen3),
			  trans_pcie->ctxt_info_gen3,
			  trans_pcie->ctxt_info_dma_addr);
	trans_pcie->ctxt_info_dma_addr = 0;
	trans_pcie->ctxt_info_gen3 = NULL;

	iwl_pcie_ctxt_info_free_fw_img(trans);

	dma_free_coherent(trans->dev, sizeof(*trans_pcie->prph_scratch),
			  trans_pcie->prph_scratch,
			  trans_pcie->prph_scratch_dma_addr);
	trans_pcie->prph_scratch_dma_addr = 0;
	trans_pcie->prph_scratch = NULL;

	dma_free_coherent(trans->dev, sizeof(*trans_pcie->prph_info),
			  trans_pcie->prph_info,
			  trans_pcie->prph_info_dma_addr);
	trans_pcie->prph_info_dma_addr = 0;
	trans_pcie->prph_info = NULL;
}