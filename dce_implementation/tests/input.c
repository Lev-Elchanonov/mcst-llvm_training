int useless(int some){
    some += 2;
    return some * some;
}

int loop(int x){
    int res = 0;
    int res_deleted = 0;
    while (x > 0){
        res += 100;
        x--;
    }
    res += res_deleted;
    return 5;
}

int complex_test_no_deleted(int x){
    int y = x + 100;
    int z = 0;
    if (x > 100){
        z = 10;
    } else {
        int b = 100;
        y = b + 5;
        z = 50;
    }
    int res = 0;
    while (z > 0) {
        res += 100;
        z--;
    }
    return x;
}
int main(){
    int x = 10;
    int res = x + 100;
    int x1 = x;
    int y1 = x1 + x1;
    int y2 = x1 + y2 + x;
    if (y1 > 100){
        x1 = x + 3;
    }
    else{
        x1 = x + 1;
    }
    return x;
}

