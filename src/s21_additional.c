#include "s21_decimal.h"

int set_bit(s21_decimal *x, int i, int k) {
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

int reverse_bit(s21_decimal *x, int i) {
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
    if (x->bits[n] ^= (1 << (i - (bit_move)))) res = check_bit(*x, i);
    return res;
}

int reverse(s21_decimal *x, int n) {
    for (int i = 0; i < 32; i++) (x->bits[n] ^= (1 << i));
    return x->bits[n];
}

int check_bit(s21_decimal x, int i) {
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
    if ((x.bits[n] & (1 << (i - (bit_move)))))
      res = 1;
    else
      res = 0;
    return res;
}

int see_bit(s21_decimal x, int mode) {  // печатает число в 0 и 1(двоичном коде), подавать децимал
    int count = 0;
    for (int j = 3 - mode; j >= 0; j--) {
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

int zero_left(s21_decimal x) {
    int i = 31, count = 0;
    while (!(x.bits[2] & (1 << i)) && i > -1) {
            count++;
            i--;
        }
    if (count > 31) {
        i = 31;
        while (!(x.bits[1] & (1 << i)) && i > -1) {
            count++;
            i--;
        }
    }
    if (count > 63) {
        i = 31;
        while (!(x.bits[0] & (1 << i)) && i > -1) {
            count++;
            i--;
        }
    }
    return count;
}

int zero_right(s21_decimal x) {  // считает количество нулей справа, подавать децимал
    int i = 0, count = 0;
    while (!(x.bits[0] & (1 << i)) && i < 32) {
            count++;
            i++;
        }
    if (count > 31) {
        i = 0;
        while (!(x.bits[1] & (1 << i)) && i < 32) {
            count++;
            i++;
        }
    }
    if (count > 63) {
        i = 0;
        while (!(x.bits[2] & (1 << i)) && i < 32) {
            count++;
            i++;
        }
    }
    return count;
}

int get_sign(s21_decimal value) {
  return check_bit(value, 127);
}

void set_sign(s21_decimal *value, int sign) {
  set_bit(value, 127, sign);
}

int scale_dec(s21_decimal *one_dec, s21_decimal *two_dec) {
    int result = 1;
    s21_decimal *one, *two;
    int degree_one = check_scale(*one_dec);
    int degree_two = check_scale(*two_dec);
    int count = (degree_one > degree_two) ? degree_one - degree_two : degree_two - degree_one;
    if (degree_one != degree_two) {
        if (degree_one < degree_two) {
            one = one_dec;
            two = two_dec;
        } else {
            one = two_dec;
            two = one_dec;
        }
        while (count && multen(one) == 0) {
            change_scale(one, 1);
            count--;
        }
        if (count != 0) {
            while (count && div_ten(two) == 0) {
                change_scale(two, -1);
                count--;
            }
        }
        if (count != 0) result = 0;
    }
    return result;
}

int multen(s21_decimal *val) {
    int error = 0;
    if (zero_left(*val) < 3) error = 1;
    if (!error) {
        s21_decimal two_val = *val;
        shift_left(val, 3);
        shift_left(&two_val, 1);
        error = sumbits(*val, two_val, val);
    }
    return error;
}

int div_ten(s21_decimal *value_1) {
  int check = 0;
  s21_decimal ten = {{10, 0, 0, 0}};
    int scale = value_1->bits[3];
    value_1->bits[3] = 0;
    if (s21_is_greater_or_equal(*value_1, ten)) {
      s21_decimal value = *value_1;
      int bit4 = value_1->bits[3];
      int count_shift = 0;
      s21_decimal minus_ten = {0};
      reverse_for_div(&value, &ten, &minus_ten, &count_shift);
      s21_decimal one = {{1, 0, 0, 0}};
      sumbits(minus_ten, one, &minus_ten);
      value = *value_1;
      check = sumbits(value, minus_ten, &value);
      int left_bit = check_bit(value, 95);
      left_bit ^= (1 << 0);
      s21_decimal result = {{0, 0, 0, 0}};
      set_bit(&result, 0, left_bit);
      shift_left(&value, 1);
      div_process(&value, ten, minus_ten, &result, &count_shift, &left_bit);
      *value_1 = result;
      value_1->bits[3] = bit4;
    } else {
    value_1->bits[0] = 0;
    value_1->bits[3] = 0;
    }
    value_1->bits[3] = scale;
    return check;
}

void null_scale(s21_decimal *x) {
    int scale = check_scale(*x);
    if (scale != 0) change_scale(x, -scale);
}

void plant_sign(s21_decimal one, s21_decimal two, int *result) {
    *result = 0;
    int one_sign = get_sign(one);
    int two_sign = get_sign(two);
    if ((one_sign == 0 && two_sign == 1) || (one_sign == 1 && two_sign == 0)) {
        *result = 1;
    }
}

void shift_left(s21_decimal *value, int i) {
    while (i--) {
        int count = 2;
        int bit = 63;
        int bit_slot = 64;
        while (count) {
            int save_bit = check_bit(*value, bit);
            value->bits[count] = value->bits[count] << 1;
            set_bit(value, bit_slot, save_bit);
            count--;
            bit -= 32;
            bit_slot -= 32;
        }
        value->bits[0] = value->bits[0] << 1;
    }
}

void shift_right(s21_decimal *value, int i) {
    while (i--) {
        int count = 2;
        int bit = 32;
        int bit_slot = 31;
        for (int j = 0; j < count; j++) {
            int save_bit = check_bit(*value, bit);
            value->bits[j] = value->bits[j] >> 1;
            set_bit(value, bit_slot, save_bit);
            bit += 32;
            bit_slot += 32;
        }
        value->bits[2] = value->bits[2] >> 1;
    }
}

int check_scale(s21_decimal value) {
    value.bits[3] = value.bits[3] << 2;
    value.bits[3] = value.bits[3] >> 18;
    return value.bits[3];
}

void change_scale(s21_decimal *value, int change_scale) {
    value->bits[3] >>= 16;
    value->bits[3] += change_scale;
    value->bits[3] <<= 16;
}

void long_shift_left(long_decimal *value, int i) {
    while (i--) {
        int count = 5;
        int bit = 159;
        int bit_slot = 160;
        while (count) {
            int save_bit = long_check_bit(*value, bit);
            value->bits[count] = value->bits[count] << 1;
            long_set_bit(value, bit_slot, save_bit);
            count--;
            bit -= 32;
            bit_slot -= 32;
        }
        value->bits[0] = value->bits[0] << 1;
    }
}

void long_shift_right(long_decimal *value, int i) {
    while (i--) {
        int count = 5;
        int bit = 32;
        int bit_slot = 31;
        for (int j = 0; j < count; j++) {
            int save_bit = long_check_bit(*value, bit);
            value->bits[j] = value->bits[j] >> 1;
            long_set_bit(value, bit_slot, save_bit);
            bit += 32;
            bit_slot += 32;
        }
        value->bits[5] = value->bits[5] >> 1;
      long_set_bit(value, LONGBITS - 1, 0);
    }
}

int long_check_scale(long_decimal value) {
    value.bits[6] = value.bits[6] << 2;
    value.bits[6] = value.bits[6] >> 18;
    return value.bits[6];
}

void long_change_scale(long_decimal *value, int change_scale) {
    value->bits[6] >>= 16;
    value->bits[6] += change_scale;
    value->bits[6] <<= 16;
}

int get_float_exp(float *src) {
    return ((*(int*)src & ~0x80000000) >> 23) - 127;
}


int null_dec(s21_decimal x) {
    x.bits[3] = 0;
    s21_decimal y = {{0, 0, 0, 0}};
    int res = s21_is_equal(x, y);
    return res;
}

int is_less_abs(s21_decimal one, s21_decimal two) {
  set_sign(&one, 0);
  set_sign(&two, 0);
  return s21_is_less(one, two);
}

int get_float_bit(float src) {
    return *(int*)&src;
}
