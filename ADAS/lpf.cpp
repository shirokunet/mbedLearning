#include "lpf.h"

void LPF::setval(double val){
  _val = val;
}

void LPF::setgain(double gain){
  _gain = gain;
}

double LPF::pass(double val){
  _val = _val * (1.0 - _gain) + val * _gain;
  return _val;
}