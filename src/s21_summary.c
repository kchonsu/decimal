#include "s21_decimal.h"

int sumbits(s21_decimal a, s21_decimal b, s21_decimal *result) {
  long_decimal summary;
  long_sum(castDecToLong(a), castDecToLong(b), &summary, 0, LONGBITS);
  result->bits[0] = summary.bits[0];
  result->bits[1] = summary.bits[1];
  result->bits[2] = summary.bits[2];
  result->bits[3] = a.bits[3];
  return overflow(summary);
}

int is_null(long_decimal src) {
  return !src.bits[0] && !src.bits[1] && !src.bits[2] && !src.bits[3] && !src.bits[4] && !src.bits[5];
}

int banker_round(long_decimal *src) {
  int code = 0;
  long_truncate(*src, src);
  if (long_check_bit(*src, 0)) {
    long_decimal one = {{1, 0, 0, 0, 0, 0, 0}};
    long_sum(*src, one, src, 0, LONGBITS);
  }
  if (is_null(*src) && long_check_scale(*src) > 0)
    code = 2;
  return code;
}

int long_multen(long_decimal *val) {
  long_decimal two_val = *val;
  long_shift_left(val, 3);
  long_shift_left(&two_val, 1);
  long_sum(*val, two_val, val, 0, LONGBITS);
  return 0;
}

int long_scale(long_decimal *a, long_decimal *b) {
  int count = long_check_scale(*a) - long_check_scale(*b);
  while (count) {
    if (count > 0) {
      if (is_greater_ten(*a)) {
        long_div_ten(a);
        long_change_scale(a, -1);
      } else {
        long_multen(b);
        long_change_scale(b, 1);
      }
    } else {
      if (is_greater_ten(*b)) {
        long_div_ten(b);
        long_change_scale(b, -1);
      } else {
        long_multen(a);
        long_change_scale(a, 1);
      }
    }
    if (count > 0)
      count--;
    else
      count++;
  }
  return 0;
}

int bank_sum(s21_decimal a, s21_decimal b, s21_decimal *result) {
  long_decimal summary, raw_a = castDecToLong(a), raw_b = castDecToLong(b);
  long_scale(&raw_a, &raw_b);
  long_sum(raw_a, raw_b, &summary, 0, LONGBITS);
  summary.bits[6] = raw_a.bits[6];
  if (long_check_scale(summary) > 0 && overflow(summary)) {
    banker_round(&summary);
  }
  result->bits[0] = summary.bits[0];
  result->bits[1] = summary.bits[1];
  result->bits[2] = summary.bits[2];
  result->bits[3] = summary.bits[6];
  return overflow(summary);
}

int sub_bits(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (is_less_abs(value_1, value_2)) {
    s21_decimal buff = value_2;
    value_2 = value_1;
    value_1 = buff;
  }
  long_decimal summary = {0};
  long_decimal raw_a = castDecToLong(value_1), raw_b = castDecToLong(value_2);
  long_scale(&raw_a, &raw_b);
  long_decimal oneval = {{1, 0, 0, 0, 0, 0, 0}};
  for (int i = 0; i < 6; i++)
    long_reverse(&raw_b, i);
  long_sum(raw_b, oneval, &raw_b, 0, LONGBITS);
  long_sum(raw_a, raw_b, &summary, 0, LONGBITS);
  summary.bits[6] = 0;
  long_change_scale(&summary, long_check_scale(raw_a));
  if (long_check_scale(summary) > 0 && overflow(summary))
    banker_round(&summary);
  result->bits[0] = summary.bits[0];
  result->bits[1] = summary.bits[1];
  result->bits[2] = summary.bits[2];
  result->bits[3] = summary.bits[6];
  set_sign(result, get_sign(value_1));
  return overflow(summary);
}

int long_sum(long_decimal a, long_decimal b, long_decimal *result, int start, int end) {
  int next_rank = 0, error = 0;
  for (int i = start; i < end; i++) {
    int bit_a = long_check_bit(a, i), bit_b = long_check_bit(b, i);
    if (bit_a && bit_b) {
      if (next_rank)
        long_set_bit(result, i, 1);
      else
        long_set_bit(result, i , 0);
      next_rank = 1;
    } else if ((bit_a && !bit_b) || (!bit_a && bit_b)) {
      if (next_rank) {
        long_set_bit(result, i , 0);
        next_rank = 1;
      } else {
        long_set_bit(result, i, 1);
        next_rank = 0;
      }
    } else if (!bit_a && !bit_b) {
      if (next_rank)
        long_set_bit(result, i, 1);
      else
        long_set_bit(result, i, 0);
      next_rank = 0;
    }
    if (next_rank && i == end - 1) error = 1;
  }
  return error;
}


int long_set_bit(long_decimal *x, int i, int k) {
  int res = -1, bit_move = 0, n = 0;
  if (i >= 32 && i <= 63) {
    n = 1;
    bit_move = 32;
  }
  if (i >= 64 && i <= 95) {
    n = 2;
    bit_move = 64;
  }
  if (i >= 96 && i <= 127) {
    n = 3;
    bit_move = 96;
  }
  if (i >= 128 && i <= 159) {
    n = 4;
    bit_move = 128;
  }
  if (i >= 160 && i <= 191) {
    n = 5;
    bit_move = 160;
  }
  if (i >= 192 && i <= 223) {
    n = 6;
    bit_move = 192;
  }
  if (k == 1) {
    x->bits[n] |= (1 << (i - (bit_move)));
    res = 1;
  }
  if (k == 0) {
    x->bits[n] &= ~(1 << (i - (bit_move)));
    res = 0;
  }
  return res;
}

int long_check_bit(long_decimal x, int i) {
  int bit_move = 0, n = -1, res = -1;
  if (i >= 0 && i <= 31) n = 0;
  if (i >= 32 && i <= 63) {
    n = 1;
    bit_move = 32;
  }
  if (i >= 64 && i <= 95) {
    n = 2;
    bit_move = 64;
  }
  if (i >= 96 && i <= 127) {
    n = 3;
    bit_move = 96;
  }
  if (i >= 128 && i <= 159) {
    n = 4;
    bit_move = 128;
  }
  if (i >= 160 && i <= 191) {
    n = 5;
    bit_move = 160;
  }
  if (i >= 192 && i <= 223) {
    n = 6;
    bit_move = 192;
  }
  if ((x.bits[n] & (1 << (i - (bit_move)))))
    res = 1;
  else
    res = 0;
  return res;
}

int long_truncate(long_decimal value, long_decimal *result) {
  int error = 0;
  *result = value;
  while (long_check_scale(*result) > 0) {
    error = long_div_ten(result);
    long_change_scale(result, -1);
  }
  return error;
}

int long_zero_left(long_decimal x) {
  int count = 0;
  for (int i = LONGBITS - 1; i <= 0; i--)
    if (!long_check_bit(x, i))
      count++;
  return count;
}

int long_reverse(long_decimal *x, int n) {
  for (int i = 0; i < 32; i++) (x->bits[n] ^= (1 << i));
  return x->bits[n];
}

long_decimal castDecToLong(s21_decimal src) {
  long_decimal dst = {0};
  for (int i = 0; i < 3; i++)
    dst.bits[i] = src.bits[i];
  dst.bits[6] = src.bits[3];
  return dst;
}

int overflow(long_decimal src) {
  return src.bits[3] || src.bits[4] || src.bits[5];
}

int is_greater_ten(long_decimal value) {
  int count = LONGBITS;
  while (long_check_bit(value, count - 1) != 1 && count) {
    count--;
  }
  return (count > 4 || value.bits[0] >= 10);
}

int castLongToDec(long_decimal src, s21_decimal *dst) {
  int code = 0, scale = long_check_scale(src);
  while (overflow(src) || scale > 28) {
    if (long_check_scale(src) > 0) {
      long_change_scale(&src, -1);
      long_div_ten(&src);
      scale--;
    } else {
      code = 1;
      break;
    }
    if (is_null(src) && scale > 28) {
      code = 2;
      break;
    }
  }
  if (overflow(src)) code = 1;
  if (!code) {
    dst->bits[0] = src.bits[0];
    dst->bits[1] = src.bits[1];
    dst->bits[2] = src.bits[2];
    dst->bits[3] = src.bits[6];
  } else {
    dst->bits[0] = dst->bits[1] = dst->bits[2] = dst->bits[3] = 0;
  }
  return code;
}

int long_count_bit(long_decimal value) {
  int count = LONGBITS;
  while (long_check_bit(value, count - 1) != 1 && count) {
    count--;
  }
  return count;
}

int long_div_ten(long_decimal *value_1) {
  int check = 0;
  long_decimal ten = {{10, 0, 0, 0, 0, 0, 0}};
  if (is_greater_ten(*value_1)) {
    long_decimal value = *value_1;
    int bit4 = value_1->bits[6];
    int count_shift = 0;
    while (long_count_bit(value) > long_count_bit(ten)) {
      long_shift_left(&ten, 1);
      count_shift++;
    }
    long_decimal minus_ten = ten;
    for (int i = 0; i < 6; i++)
      long_reverse(&minus_ten, i);
    long_decimal one = {{1, 0, 0, 0, 0, 0, 0}};
    long_decimal sum = {0};
    check = long_sum(minus_ten, one, &sum, 0, LONGBITS);
    minus_ten = sum;
    value = *value_1;
    check = long_sum(value, minus_ten, &sum, 0, LONGBITS);
    value = sum;
    int left_bit = long_check_bit(value, LONGBITS - 1);
    left_bit ^= (1 << 0);
    long_decimal result = {0};
    long_set_bit(&result, 0, left_bit);
    long_shift_left(&value, 1);
    while (count_shift > 0) {
      if (left_bit == 1) {
        check = long_sum(value, minus_ten, &sum, 0, LONGBITS);
        value = sum;
      }
      if (left_bit == 0) {
        check = long_sum(value, ten, &sum, 0, LONGBITS);
        value = sum;
      }
      left_bit = long_check_bit(value, LONGBITS - 1);
      left_bit ^= (1 << 0);
      long_shift_left(&result, 1);
      long_set_bit(&result, 0, left_bit);
      long_shift_left(&value, 1);
      count_shift--;
    }
    *value_1 = result;
    value_1->bits[6] = bit4;
    return check;
  } else {
    value_1->bits[0] = 0;
    value_1->bits[6] = 0;
  }
  return check;
}


int long_see_bit(long_decimal x, int mode) {
  int count = 0;
  for (int j = 6 - mode; j >= 0; j--) {
    for (int i = 31; i >= 0; i--) {
      if (x.bits[j] & (1 << i))
        printf("1");
      else
        printf("0");
      count++;
      if (count == 4) {
        if (!mode)
          printf(" ");
        count = 0;
      }
    }
    if (!mode)
      printf("\n");
  }
  return 0;
}
