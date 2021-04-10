
struct Mat {
    int n, m;
    vector<vector<pair<int,ld>>> mat;
    ld zero = 0;
    ld &operator()(int i, int j = 0) {
        int l = 0, h = mat[i].size();
        if(h == 0){
            mat[i].push_back({j, 0});
        }
        while(h - l > 1){
            int mid = (h + l) / 2;
            if(mat[i][mid].first > j){
                h = mid;
            }
            else{
                l = mid;
            }
        }
        if(mat[i][l].first != j){
            if(l==0 && mat[i][l].first > j){
                mat[i].insert(mat[i].begin(), {j, 0});
            }
            else{
                mat[i].insert(mat[i].begin() + l + 1, {j, 0});
                l++;
            }
        }
        return mat[i][l].second;
    }
    const ld &operator()(int i, int j = 0) const {
        int l = 0, h = mat[i].size();
        if(h == 0){
            return zero;
        }
        while(h - l > 1){
            int mid = (h + l) / 2;
            if(mat[i][mid].first > j){
                h = mid;
            }
            else{
                l = mid;
            }
        }
        if(mat[i][l].first == j){
            return mat[i][l].second;
        }
        else{
            return zero;
        }
    }   
    static Mat cero(int _n, int _m = 1) {
        Mat res { _n, _m, vector<vector<pair<int,ld>>>(_n) };
        return res;
    }
    static Mat identidad(int _n) {
        Mat res = Mat::cero(_n, _n);
        for(int i = 0; i < res.n; i++){
            res(i, i) = 1;
        }
        return res;
    }
    Mat extender(Mat  A){
        assert(A.m==1 && A.n == n);
        auto res = Mat::cero(n, m + 1);
        for(int i = 0; i < n; i++){
            for(auto x: mat[i]){
                res.mat[i].push_back({x.first,x.second});
            }
        }
        for(int i = 0; i < m; i++){
            res.mat[i].push_back({m,A(i)}); 
        }
        return res;
    }
    Mat transpuesta(){
        auto res = Mat::cero(m, n);
        for(int i = 0; i < n; i++){
            for(auto x: mat[i]){
                res.mat[x.first].push_back({i, x.second});
            }
        }
        return res;
    }
    void operacion_2(int fila_1, int fila_2, ld multiplo){
        for(auto x : mat[fila_1]){
            int j = x.first;
            int l = 0, h = mat[fila_2].size();
            if(h == 0){
                mat[fila_2].push_back({j, 0});
            }
            while(h - l > 1){
                int mid = (h + l) / 2;
                if(mat[fila_2][mid].first > j){
                    h = mid;
                }
                else{
                    l = mid;
                }
            }
            if(mat[fila_2][l].first != j){
                if(l==0 && mat[fila_2][l].first > j){
                    mat[fila_2].insert(mat[fila_2].begin(), {j, 0});
                }
                else{
                    mat[fila_2].insert(mat[fila_2].begin() + l + 1, {j, 0});
                    l++;
                }
            }
            mat[fila_2][l].second = mat[fila_2][l].second + x.second * multiplo;
            if(mat[fila_2][l].second == 0){
                mat[fila_2].erase(mat[fila_2].begin()+l);
            }
        }
    }
    void operacion_3(int fila_1, int fila_2){
        swap(mat[fila_1], mat[fila_2]);
    }
};