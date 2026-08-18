#include <stdio.h>


// эта функция пометится как memory(none) + nounwind - вызов будет удален
int LinearFunc(int x){
    int y = x + 100;        // должен удалить
    int z = y + 100;        // должен удалить
    int r = z + 100;        // должен удалить
    return x;
}

// эта функция пометится как memory(none) + nounwind - вызов будет удален
int BranchsFunc(int x){
    int y = x + 100;      // должен удалить
    if (x > 5) {
        y = y + 5;        // должен удалить
    } else {
        x = 3;
    }
    return x;
}

// в LoopFunc не должно быть удаленных переменных, из-за возникающих phi-узлов в хедере цикла
// эта функция пометится как memory(none) + nounwind - вызов будет удален
int LoopFunc(int x){
    int y = x + 100;
    while (x > 0){
        y = y + 5;
        x--;
    }
    return 5;
}

// эта функция пометится как memory(none) + nounwind - вызов будет удален
int ReadNoneFunc(int x){
    int y = x + 100;        // должен удалить
    return 100;
}

// эта функция пометится как memory(argmem: read) - вызов будет удален
int ReadOnlyFunc(int* ptr){
    int res = *ptr;        // читаем из памяти
    return res;
}

// нельзя удалить вызов, так как есть запись в память
int WriteFunc(int* ptr){
    *ptr = 100;       // не будет удалена, так как это store
    return *ptr;
}

// нельзя удалить, так как функция может кинуть исключение
int ExceptionFunc(int x){
    if (x < 0){
        throw 100;
    }
    return 4;
}

// нельзя удалить, так как функция имеет побочный эффект
int PrintFunc(int x){
    printf("This function prints something: %d\n", x);
    return x;
}

int main(){
    int x = 10;
    int noUse = LinearFunc(x);    // должен удалить, так как результат не используется
    int noUse2 = BranchsFunc(x);  // должен удалить, так как результат не используется

    int Use = LoopFunc(x);
    x += Use;
    int noUse3 = LoopFunc(x);     // должен удалить, так как результат не используется

    int noUse4 = ReadNoneFunc(x); // должен удалить, так как вызов чистый и не используется
    int noUse5 = ReadOnlyFunc(&x); // должен удалить, так как вызов чистый и не используется

    int noUseWrite = WriteFunc(&x); // не может удалить, так как есть запись в память. Функция не pure

    int noUse6 = ExceptionFunc(100); // не может удалить, так как функция может кинуть исключение
    int noUse7 = PrintFunc(x);       // не может удалить, так как функция пишет на экран

    return x;
}