#include<bits/stdc++.h>
#include<unistd.h>
#include<pthread.h>

using namespace std;

// int cnt;
// pthread_mutex_t mtx;

typedef struct {
    int* cnt;
    int nr_exp;
    int id;
    pthread_barrier_t *bar;
    pthread_mutex_t *mtx;
    vector<string> files;
    vector<vector<long long>> result;
} my_struct;

typedef struct {
    int id;
    int *cnt;
    pthread_barrier_t *bar;
    pthread_mutex_t *mtx;
    vector<my_struct> tmps;
} reduce_struct;


//vector<vector<vector<long long>>> result;

long long power(long long a, long long b) {
    long long res = 1;
    for (int i = 1; i <= b; i++) {
        res *= a;
    }

    return res;
}

void binary_search(string filename, int max_exp, vector<long long> &search, vector<vector<long long>> &res) {
    //cout << "in binary search\n";
    int n = search.size() - 1;
    for (int exp = 2; exp <= max_exp; exp++) {
        int left = 0, right = n;
        while (left <= right) {
            int mid = (left + right) / 2;
            long long pow_ans = power(search[mid], exp);


            if (pow_ans == search[n]) {
                res[exp].push_back(search[n]);
                break;
            }

            if (pow_ans < search[n]) {
                left = mid + 1;
            }

            if (pow_ans > search[n]) {
                right = mid - 1;
            }
        }
    }
}

void *map_func(void *arg) {
    //cout << "aici\n";
    my_struct *tmp = (my_struct *)arg;
    //vector<vector<long long>> result(tmp->nr_exp + 1);

    while (*(tmp->cnt) < tmp->files.size()) {
        int r = pthread_mutex_lock(tmp->mtx);
        if (r) {
            cout << "mutex_lock'\n";
        }

        int count = *(tmp->cnt);

        *(tmp->cnt) = *(tmp->cnt) + 1;

        r = pthread_mutex_unlock(tmp->mtx);
        if (r) {
            cout << "mutex_unlock\n";
        }

        if (count >= tmp->files.size()) {
            pthread_mutex_unlock(tmp->mtx);
            pthread_exit(NULL);
            return NULL;
        }

        cout << "Threadul " << tmp->id << " se ocupa de fisierul " << tmp->files[count] << '\n';

        ifstream fin(tmp->files[count]);
        int nr_tests;
        fin >> nr_tests;

        for (int i = 0; i < nr_tests; i++) {
            int number;
            fin >> number;

            vector<long long> search;
            for (int j = 1; j <= number; j++) {
                search.push_back(j);
            }

            binary_search(tmp->files[count], tmp->nr_exp, search, tmp->result);
        }
    }

    //pthread_barrier_wait(tmp->bar);
    pthread_exit(NULL);
    return NULL;
}

void *reduce_func(void *arg) {
    reduce_struct reduce = *(reduce_struct*)arg;
    vector<my_struct> mappers = reduce.tmps;

    //cout << "aici\n";
    pthread_barrier_wait(reduce.bar);

    //pt fiecare exponent, parcurg threadurile mapper
    for (int i = 0; i < mappers.size(); i++) {
        my_struct current_mapper = mappers[i];
        unordered_map<int, int> umap;

        //pt fiecare mapper, parcurg lista corespunzatoare exponentului
        for (auto &x : current_mapper.result[reduce.id + 2]) {
            umap[x] = 1;
        }

        string out_file = "out" + to_string(reduce.id + 2) + ".txt";
        ofstream fout(out_file);

        for (auto it = umap.begin(); it != umap.end(); it++) {
            fout << it->first << ' ';
        }

        //fout << umap.size() << '\n';
    }

    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[]) {
    int nr_map = atoi(argv[1]);
    int nr_reduce = atoi(argv[2]);
    string original_file = argv[3];

    fstream fin;
    fin.open(original_file);

    string line;
    getline(fin, line);
   
    vector<string> files;
    while (getline(fin, line)) {
        files.push_back(line);
    }

    fin.close();

    int total_threads = nr_map + nr_reduce;
    vector<pthread_t> threads(nr_map);
    vector<int> thread_id(total_threads);

    pthread_mutex_t *mtx = new pthread_mutex_t();
    pthread_mutex_init(mtx, NULL);

    pthread_barrier_t *bar = new pthread_barrier_t();
    pthread_barrier_init(bar, NULL, total_threads);

    int *cnt = new int();
    *cnt = 0;

    //result = vector<vector<vector<long long>>>(nr_map, vector<vector<long long>>(nr_reduce + 2));

    vector<my_struct> tmps;
    for (int i = 0; i < nr_map; i++) {
        my_struct tmp;
        tmp.id = i;
        tmp.cnt = cnt;
        tmp.nr_exp = nr_reduce + 1;
        tmp.files = files;
        tmp.mtx = mtx;
        tmp.bar = bar;
        tmp.result = vector<vector<long long>>(nr_reduce + 2);
        tmps.push_back(tmp);
    }

    int *cnt2 = new int();
    *cnt2 = 0;

    vector<reduce_struct> rdc;
    for (int i = 0; i < nr_reduce; i++) {
        reduce_struct rd;
        rd.id = i;
        rd.cnt = cnt2;
        rd.mtx = mtx;
        rd.bar = bar;
        rdc.push_back(rd);
    }

    void *status;

    for (int i = 0; i < nr_map; i++) {
        if (i < nr_map) {
            pthread_create(&threads[i], NULL, map_func, &tmps[i]);
        } //else {
        //     rdc[i - nr_map].tmps = tmps;
        //     pthread_create(&threads[i], NULL, reduce_func, &rdc[i - nr_map]);
        // }
    }

    for (int i = 0; i < nr_map; i++) {
        pthread_join(threads[i], &status);
    }

    
    pthread_mutex_destroy(mtx);
    pthread_barrier_destroy(bar);
    return 0;
}
