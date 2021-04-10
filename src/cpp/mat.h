struct Mat {
    int n, m;
    vector<vector<ld>> mat;
    ld &operator()(int i, int j = 0) {
        return mat[i][j];  
    }
    const ld &operator()(int i, int j = 0) const {
        return mat[i][j];  
    }
    static Mat cero(int _n, int _m = 1) {
        Mat res { _n, _m, vector<vector<ld>>(_n, vector<ld>(_m, 0)) };
        return res;
    }
    static Mat identidad(int _n) {
        Mat res = Mat::cero(_n, _n);
        for(int i = 0; i < res.n; i++){
            res(i, i) = 1;
        }
        return res;
    }
    Mat extender(Mat A){
        assert(A.m==1 && A.n == n);
        auto res = Mat::cero(n, m + 1);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                res(i, j) = mat[i][j];
            }
        }
        for(int i = 0; i < m; i++){
            res(i, m) = A(i); 
        }
        return res;
    }
    Mat transpuesta(){
        auto res = Mat::cero(m, n);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                res(j, i) = mat[i][j];
            }
        }
        return res;
    }
    void operacion_2(int fila_1, int fila_2, ld multiplo){
        for(int i = 0; i < m; i++){
            mat[fila_2][i] = mat[fila_2][i] + mat[fila_1][i] * multiplo;
        }
    }
    void operacion_3(int fila_1, int fila_2){
        swap(mat[fila_1], mat[fila_2]);
    }
};