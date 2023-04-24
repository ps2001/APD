#include<bits/stdc++.h>

using namespace std;

void modify(int n, int *v) {
    for (int i = 0; i < n; i++) {
        v[i] = 100;
    }
}

int main() {
    int n;
    cin >> n;

    int *v = (int *)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        cout << v[i] << ' ';
    }
    cout << '\n';

    modify(n, v);

    for (int i = 0; i < n; i++) {
        cout << v[i] << ' ';
    }
    cout << '\n';
}