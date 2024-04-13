fuji_decode_sample_odd(struct fuji_compressed_block *info,
                       const struct fuji_compressed_params *params,
                       ushort *line_buf, int pos, struct int_pair *grads)
{
  int interp_val = 0;
  int errcnt = 0;

  int sample = 0, code = 0;
  ushort *line_buf_cur = line_buf + pos;
  int Ra = line_buf_cur[-1];
  int Rb = line_buf_cur[-2 - params->line_width];
  int Rc = line_buf_cur[-3 - params->line_width];
  int Rd = line_buf_cur[-1 - params->line_width];
  int Rg = line_buf_cur[1];

  int grad, gradient;

  grad = fuji_quant_gradient(params, Rb - Rc, Rc - Ra);
  gradient = _abs(grad);

  if ((Rb > Rc && Rb > Rd) || (Rb < Rc && Rb < Rd))
    interp_val = (Rg + Ra + 2 * Rb) >> 2;
  else
    interp_val = (Ra + Rg) >> 1;

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
    interp_val -= code;
  else
    interp_val += code;
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