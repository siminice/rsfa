int MIN(int x, int y) {
  if (x<y) return x;
  else return y;
}
  
int dist(char*a, char*b) {
  int i, j, modif, ins, del;
  int m = strlen(a);
  int n = strlen(b);
  int **d = new int*[m+1];
  for (i=0; i<=m; i++) {
    d[i] = new int[n+1];
    d[i][0] = i;
  }
  for (i=0; i<=n; i++)
    d[0][i] = i;

  for (i=1; i<=m; i++)  
    for (j=1; j<=n; j++) {
      if (a[i-1] == b[j-1]) modif = d[i-1][j-1];
        else modif = d[i-1][j-1] + 1;
      del = MIN(d[i-1][j], d[i][j-1]) + 1;
      d[i][j] = MIN(modif, del);
    }

  int res = d[m][n];

  // cleanup;
  for (int i=0; i<=m; i++)
    delete[] d[i];
  delete[] d;
  return res;
}
