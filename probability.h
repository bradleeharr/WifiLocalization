float calculateMean(int rssi[], int count) {
  float sum = 0;
  for (int i = 0; i < count; i++) {
    sum += rssi[i];
  }
  return sum / count;
}

float calculateVariance(int rssi[], int count, float mean) {
  float sum = 0;
  for (int i = 0; i < count; i++) {
    sum += pow(rssi[i] - mean, 2);
  }
  return sum / count;
}
