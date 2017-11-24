scale(int i[], double k[]) {
    
  k[0] |= k[1];
  i[1] |= k[2];
  k[2] |= i[2];
  i |= i;

  return i;
}
