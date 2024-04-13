fuji_decode_sample_even(struct fuji_compressed_block *info,
                        const struct fuji_compressed_params *params,
                        ushort *line_buf, int pos, struct int_pair *grads)
{
  int interp_val = 0;
  // ushort decBits;
  int errcnt = 0;

  int sample = 0, code = 0;
  ushort *line_buf_cur = line_buf + pos;
  int Rb = line_buf_cur[-2 - params->line_width];
  int Rc = line_buf_cur[-3 - params->line_width];
  int Rd = line_buf_cur[-1 - params->line_width];
  int Rf = line_buf_cur[-4 - 2 * params->line_width];

  int grad, gradient, diffRcRb, diffRfRb, diffRdRb;

  grad = fuji_quant_gradient(params, Rb - Rf, Rc - Rb);
  gradient = _abs(grad);
  diffRcRb = _abs(Rc - Rb);
  diffRfRb = _abs(Rf - Rb);
  diffRdRb = _abs(Rd - Rb);

  if (diffRcRb > diffRfRb && diffRcRb > diffRdRb)
    interp_val = Rf + Rd + 2 * Rb;
  else if (diffRdRb > diffRcRb && diffRdRb > diffRfRb)
    interp_val = Rf + Rc + 2 * Rb;
  else
    interp_val = Rd + Rc + 2 * Rb;

  fuji_zerobits(info, &sample);

  if (sample < params->max_bits - params->raw_bits - 1)
  {
    int decBits = bitDiff(grads[gradient].value1, grads[gradient].value2);
    fuji_read_code(info, &code, decBits);
    code += sample << decBits;
  }
  else
  {
    fuji_read_code(info, &code, params->raw_bits);
    code++;
  }

  if (code < 0 || code >= params->total_values)
    errcnt++;

  if (code & 1)
    code = -1 - code / 2;
  else
    code /= 2;

  grads[gradient].value1 += _abs(code);
  if (grads[gradient].value2 == params->min_value)
  {
    grads[gradient].value1 >>= 1;
    grads[gradient].value2 >>= 1;
  }
  grads[gradient].value2++;
  if (grad < 0)
    interp_val = (interp_val >> 2) - code;
  else
    interp_val = (interp_val >> 2) + code;
  if (interp_val < 0)
    interp_val += params->total_values;
  else if (interp_val > params->q_point[4])
    interp_val -= params->total_values;

  if (interp_val >= 0)
    line_buf_cur[0] = _min(interp_val, params->q_point[4]);
  else
    line_buf_cur[0] = 0;
  return errcnt;
}