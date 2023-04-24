#include<bits/stdc++.h>
#include<pthread.h>

using namespace std;

typedef struct {
    int* cnt;
    int nr_exp;
    int nr_map;
    int nr_reduce;
    pthread_barrier_t *bar;
    pthread_mutex_t *mtx;
    vector<string> files;
    long long*** result;
} my_struct;

typedef struct {
    my_struct *tmp;
    int id;
} mapper_struct;

typedef struct {
    int *cnt;
    int nr_exp;
    int nr_map;
    int nr_reduce;
    pthread_mutex_t *mtx;
    pthread_barrier_t *bar;
} reduce_struct;

typedef struct {
    int id;
    reduce_struct *rdc;
} my_reduce;


//functie ce calculeaza a la puterea b
//pentru a nu obtine overflow, in cazul in care rezultatul partial depaseste target
//returneaza target + 1
long long power(long long a, long long b, int target) {
    long long res = 1;
    for (int i = 1; i <= b; i++) {
        if (res > target) {
            return target + 1;
        }
        res *= a;
    }

    return res;
}

vector<vector<vector<long long>>> debug;

//functie de cautare binara
//pentru fiecare exponent, cauta binar intre numerele de la 1 la target
//un numar care ridicat la exponentul curent este egal cu target
void binary_search(string filename, int target, int id, int max_exp) {
    for (int exp = 2; exp <= max_exp; exp++) {
        int left = 1, right = target;
        while (left <= right) {
            int mid = (left + right) / 2;
            long long pow_ans = power(mid, exp, target);

            if (pow_ans == target) {
                debug[id][exp - 2].push_back(target);
                break;
            }

            if (pow_ans < target) {
                left = mid + 1;
            }

            if (pow_ans > target || pow_ans < 0) {
                right = mid - 1;
            }
        }
    }
}

void *map_func(void *arg) {
    mapper_struct str = *(mapper_struct *)arg;
    my_struct *tmp = str.tmp;

    //cat timp mai avem fisiere de procesat
    while (*(tmp->cnt) < tmp->files.size()) {
        //blocheaza accesul la lista de fisiere pt celalalte thread uri
        int r = pthread_mutex_lock(tmp->mtx);
        if (r) {
            cout << "mutex_lock'\n";
        }

        //marcheaza fisierul ca fiind prelucrat
        int count = *(tmp->cnt);

        *(tmp->cnt) = *(tmp->cnt) + 1;

        //deblocheaza accesul la lista de fisiere pt celalalte threaduri
        r = pthread_mutex_unlock(tmp->mtx);
        if (r) {
            cout << "mutex_unlock\n";
        }


        if (count >= tmp->files.size()) {
            break;
        }

        fstream file;
        file.open(tmp->files[count], ios::in);

        int nr_tests;
        file >> nr_tests;

        //pt fiecare numar din fisier, verifica pt ce exponenti este putere perfecta
        for (int i = 0; i < nr_tests; i++) {
            int number;
            file >> number;
            binary_search(tmp->files[count], number, str.id, tmp->nr_exp);
        }

        file.close();
    }
 
    pthread_barrier_wait(tmp->bar);
    pthread_exit(NULL);
    return NULL;
}

void *reduce_func(void *arg) {
    my_reduce mrd = *(my_reduce*)arg;
    reduce_struct *rdc = mrd.rdc;

    pthread_barrier_wait(rdc->bar);

    //pt fiecare mapper, parcurge lista de puteri perfecte corespunzatoare id-ului mapperului curent
    unordered_map<long long, int> umap;
    for (int i = 0; i < rdc->nr_map; i++) {
        for (int j = 0; j < debug[i][mrd.id].size(); j++) {
            umap[debug[i][mrd.id][j]] = 1;
        }
    }

    //scrie nr de puteri perfecte in fisierul de iesire
    string out_file = "out" + to_string(mrd.id + 2) + ".txt";
    fstream file;
    file.open(out_file, ios::out);


    file << umap.size();
    file.close();

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
    vector<pthread_t> threads(total_threads);
    vector<int> thread_id(total_threads);

    pthread_mutex_t *mtx = new pthread_mutex_t();
    pthread_mutex_init(mtx, NULL);

    pthread_barrier_t *bar = new pthread_barrier_t();
    pthread_barrier_init(bar, NULL, total_threads);

    debug = vector<vector<vector<long long>>>(nr_map, vector<vector<long long>>(nr_reduce));

    int *cnt = new int();
    *cnt = 0;

    my_struct tmp;
    tmp.cnt = cnt;
    tmp.nr_exp = nr_reduce + 1;
    tmp.files = files;
    tmp.mtx = mtx;
    tmp.bar = bar;
    tmp.nr_map = nr_map;
    tmp.nr_reduce = nr_reduce;

    vector<mapper_struct> v;
    for (int i = 0; i < nr_map; i++) {
        mapper_struct str;
        str.tmp = &tmp;
        str.id = i;
        v.push_back(str);
    }

    int *cnt2 = new int();
    *cnt2 = 0;

    reduce_struct rdc;
    rdc.cnt = cnt2;
    rdc.nr_exp = nr_reduce + 1;
    rdc.nr_map = nr_map;
    rdc.nr_reduce = nr_reduce;
    rdc.bar = bar;
    rdc.mtx = mtx;

    vector<my_reduce> w;
    for (int i = 0; i < nr_reduce; i++) {
        my_reduce mrd;
        mrd.rdc = &rdc;
        mrd.id = i;
        w.push_back(mrd);
    }

    void *status;

    for (int i = 0; i < total_threads; i++) {
        if (i < nr_map) {
            pthread_create(&threads[i], NULL, map_func, &v[i]);
        } else {
            pthread_create(&threads[i], NULL, reduce_func, &w[i - nr_map]);
        }
    }


    for (int i = 0; i < total_threads; i++) {
        pthread_join(threads[i], &status);
    }

    pthread_mutex_destroy(mtx);
    pthread_barrier_destroy(bar);

    delete cnt;
    delete cnt2;
    return 0;
}
