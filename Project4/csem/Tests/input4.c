scale() {
  int i;
  double k;
  k = 6;
  i = 1;
    
  k += k;
  i += k;
  k += i;
  i += i;

  k /= k;
  i /= k;
  k /= i;
  i /= i;
  
  return i;
}
