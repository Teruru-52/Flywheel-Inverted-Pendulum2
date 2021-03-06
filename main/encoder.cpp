#include "encoder.hpp"

Encoder::Encoder(uint8_t enc_a, uint8_t enc_b)
    : enc_a(enc_a),
      enc_b(enc_b),
      count(0)
{
  pinMode(enc_a, INPUT);
  pinMode(enc_b, INPUT);
}

void Encoder::ReadEncoder()
{
  byte cur = (!digitalRead(enc_b) << 1) + !digitalRead(enc_a);
  byte old = pos & B00000011;
  byte dir = (pos & B00110000) >> 4;

  if (cur == 3)
    cur = 2;
  else if (cur == 2)
    cur = 3;

  if (cur != old) //チャタリング防止
  {
    if (dir == 0) //回転開始と終了、方向を示す判定
    {
      if (cur == 1 || cur == 3)
        dir = cur;
    }
    else
    {
      if (cur == 0)
      {
        if (dir == 1 && old == 3)
          count--;
        else if (dir == 3 && old == 1)
          count++;
        dir = 0;
      }
    }

    bool rote = 0; //回転方向
    if (cur == 3 && old == 0)
      rote = 0;
    else if (cur == 0 && old == 3)
      rote = 1;
    else if (cur > old)
      rote = 1;

    pos = (dir << 4) + (old << 2) + cur;
  }
}
