__nfp_flower_non_repr_priv_put(struct nfp_flower_non_repr_priv *non_repr_priv)
{
	if (--non_repr_priv->ref_count)
		return;

	list_del(&non_repr_priv->list);
	kfree(non_repr_priv);
}