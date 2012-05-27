#include <cmath>
#include <ctime>
#include <cstdio>
#include <string>
#include <vector>
#include <cassert>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <sys/time.h>

using std::max;
using std::min;
using std::fill;
using std::copy;
using std::sort;
using std::pair;
using std::vector;
using std::string;
using std::make_pair;

typedef long long llint;

timeval startti, endti;
llint startll, endll;

const llint MSEC_PER_SEC = 1000000LL;

#define USE_DOUBLE 1

#if USE_DOUBLE
typedef double real;
#define sqrt_ sqrt
#define fabs_ fabs
#define pow_ pow
#else
typedef float real;
// #define sqrt_ sqrtf
#define fabs_ fabsf
inline float sqrt_(float x){
	union {
		int intPart;
		float floatPart;
	} convertor;
	union {
		int intPart;
		float floatPart;
	} convertor2;
	convertor.floatPart = x;
	convertor2.floatPart = x;
	convertor.intPart = 0x1FBCF800 + (convertor.intPart >> 1);
	convertor2.intPart = 0x5f3759df - (convertor2.intPart >> 1);
	return 0.5f * (convertor.floatPart + (x * convertor2.floatPart));
}
#define pow_ powf
#define round_ roundf
#endif

#ifdef __WATASHI__
const llint TIME_LIMIT = 3868686;
#else
const llint TIME_LIMIT = 9543210;
#endif

void startTimer() {
    gettimeofday(&startti, NULL);
    startti.tv_sec += TIME_LIMIT / MSEC_PER_SEC;
    startti.tv_usec += TIME_LIMIT % MSEC_PER_SEC;
    startll = startti.tv_sec * MSEC_PER_SEC + startti.tv_usec;
}

void updateTimer() {
    gettimeofday(&endti, NULL);
    endll = endti.tv_sec * 1000000LL + endti.tv_usec;
}

real timeRatio() {
    updateTimer();
    return (real)(endll - startll + TIME_LIMIT) / TIME_LIMIT;
}

bool timeout() {
    updateTimer();
    return endll > startll;
}

template<typename T>
T sqr(T x) {
    return x * x;
}

template<typename T>
void split(const string& s, T* ret) {
    const char *p = s.c_str();
    while (*p != '\0') {
        // puts(p);
        char *q;
        *ret++ = strtol(p, &q, 10);
        p = q;
    }
}

const int MAXN = 2012;
const int MAXZ = 1000;
const int DIMENSION = 10;

template<typename T>
struct Point {
    T p[DIMENSION];

    void clear() {
        fill(p, p + DIMENSION, 0);
    }

    void std() {
        for (int i = 0; i < DIMENSION; ++i) {
            if (p[i] < 0) {
                p[i] = 0;
            } else if (p[i] > MAXZ) {
                p[i] = MAXZ;
            }
        }
    }

    T& operator[](int i) {
        return p[i];
    }

    const T& operator[](int i) const {
        return p[i];
    }

    void parse(const string& s) {
        split(s, p);
    }

    string str() const {
        static char buf[4096];
        char* s = buf;
        for (int i = 0; i < DIMENSION; ++i) {
            s += sprintf(s, "%d ", p[i]);
        }
        *(s - 1) = '\0';
        return buf;
        /*
        std::ostringstream oss;
        for (int i = 0; i < DIMENSION; ++i) {
            if (i > 0) {
                oss << ' ';
            }
            oss << p[i];
        }
        return oss.str();
        */
    }

    Point operator-() const {
        Point ret;
        for (int i = 0; i < DIMENSION; ++i) {
            ret[i] = -p[i];
        }
        return ret;
    }

    Point operator*(T q) const {
        Point ret;
        for (int i = 0; i < DIMENSION; ++i) {
            ret[i] = p[i] * q;
        }
        return ret;
    }

    template<typename S>
    Point operator+(const Point<S>& q) const {
        Point ret;
        for (int i = 0; i < DIMENSION; ++i) {
            ret[i] = p[i] + q[i];
        }
        return ret;
    }

    template<typename S>
    Point operator-(const Point<S>& q) const {
        Point ret;
        for (int i = 0; i < DIMENSION; ++i) {
            ret[i] = p[i] - q[i];
        }
        return ret;
    }

    T abs2() const {
        T ret = 0;
        for (int i = 0; i < DIMENSION; ++i) {
            ret += ::sqr(p[i]);
        }
        return ret;
    }

    real abs() const {
        return sqrt_(abs2());
    }
};

int LIMIT = 100;
// const int LIMIT = 100;
const int MAXSTEP = MAXN;
const real RATE = 1.0;
const real GRAVITY = 1.0;

int n;
int cz[MAXN];
Point<real> p[MAXN];
Point<int> q[MAXN];
real mind[MAXN], maxd[MAXN];
int rank[MAXN][MAXN], rank0[MAXN][MAXN];
vector<int> minDist, maxDist;

void adjust(int n, int k, real rate = 1.0) {
    static Point<real> w[MAXN];
    static real dw[MAXN], dd[MAXN];
    for (int i = 0; i < n; ++i) {
        // w[i] = p[i] - p[k];
        // dd[i] = dw[i] = w[i].abs();
        dw[i] = 0;
        for (int j = 0; j < DIMENSION; ++j) {
            w[i][j] = p[i][j] - p[k][j];
            dw[i] += w[i][j] * w[i][j];
        }
        dd[i] = dw[i] = sqrt_(dw[i]);
    }
    sort(dd, dd + n);

    real s = 0.0;
    Point<real> v;
    v.clear();
    for (int i = 0; i < n; ++i) {
        if (i == k) {
            continue;
        }
        real t = 0.0;
        if (dw[i] < mind[k]) {
            t += dw[i] - mind[k];
        } else if (dw[i] > maxd[k]) {
            t += dw[i] - maxd[k];
        }
        t += dw[i] - dd[rank[k][i]];
        if (t != 0) {
            real u = cz[i] * sqrt_(fabs_(t));
            s += u;
            t *= u / dw[i];
            // v = v + w[i] * t;
            for (int j = 0; j < DIMENSION; ++j) {
                v[j] += w[i][j] * t;
            }
        }
    }

    if (s > 0) {
        for (int i = 0; i < DIMENSION; ++i) {
            if (q[k][i] == -1) {
                p[k][i] += v[i] / s * rate;
                if (p[k][i] < 0) {
                    p[k][i] = 0;
                } else if (p[k][i] > MAXZ) {
                    p[k][i] = MAXZ;
                }
            }
        }
    }
}

template<typename T>
void shuffle(int n, T a[MAXN], const vector<pair<int, int> >& todo) {
    static T tmp[MAXN];
    for (int i = 0; i < n; ++i) {
        tmp[i] = a[todo[i].second];
    }
    copy(tmp, tmp + n, a);
}

void rerank(int k, int n, const vector<pair<int, int> >& todo) {
    pair<int, int> v[MAXN];
    for (int i = 0; i < n; ++i) {
        v[i] = make_pair(rank0[todo[k].second][todo[i].second], i);
    }
    sort(v, v + n);
    for (int i = 0; i < n; ++i) {
        if (i == 0 || v[i].first != v[i - 1].first) {
            rank[k][v[i].second] = i;
        } else {
            rank[k][v[i].second] = rank[k][v[i - 1].second];
        }
    }
}

void guess() {
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < DIMENSION; ++k) {
            if (q[i][k] == -1) {
                real s = 0.0f;
                p[i][k] = 0.0f;
                for (int j = 0; j < LIMIT; ++j) {
                    if (q[j][k] != -1) {
                        s += n - rank[i][j];
                        p[i][k] += (n - rank[i][j]) * q[j][k];
                    }
                }
                p[i][k] /= s;
            } else {
                p[i][k] = q[i][k];
            }
        }
    }
}

const int REP = 32;
const int CHECK = 64;

void gao() {
    vector<pair<int, int> > todo;
    for (int i = 0; i < n; ++i) {
        cz[i] = 0;
        for (int j = 0; j < DIMENSION; ++j) {
            if (q[i][j] == -1) {
                ++cz[i];
            }
        }
        todo.push_back(make_pair(cz[i], i));
    }

    sort(todo.begin(), todo.end());
    for (int i = 0; i < n; ++i) {
        ::cz[i] = 12 - todo[i].first;
    }

    shuffle(n, mind, todo);
    shuffle(n, maxd, todo);
    shuffle(n, q, todo);
    guess();
    /*
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < DIMENSION; ++j) {
            if (q[i][j] == -1) {
                p[i][j] = MAXZ / 2 + 50 * (2.0 * rand() / RAND_MAX - 1.0);
            } else {
                p[i][j] = q[i][j];
            }
        }
    }
    */

    int cc = 0;
    int pre = 0;
    while (!timeout()) {
        int n = lower_bound(todo.begin(), todo.end(), make_pair(cc, -1)) - todo.begin();
        int m = lower_bound(todo.begin(), todo.end(), make_pair(cc + 1, -1)) - todo.begin();
        real rate;
        /*
        if (n >= LIMIT) {
            n = LIMIT;
            m = (int)todo.size();
            rate = 12.0f;
        } else {
            m = std::min(m, LIMIT);
            rate = 8.0f;
        }
        */
        n = timeRatio() * (real)1.1 * ::n;
        n = max(n, pre + 10);
        n = min(n, pre + 100);
        n = min(n, LIMIT);
        pre = n;
        rate = M_E;
        // fprintf(stderr, "%d_: [%d, %d)\n", cc, n, m);

REDO:
        // partA
        // fprintf(stderr, "%dA: %lf\n", cc, (double)clock() / CLOCKS_PER_SEC);
        if (timeout()) {
            goto RET;
        }
        for (int i = 0; i < ::n; ++i) {
            rerank(i, n, todo);
        }
        for (int i = 0; i < REP * 2; ++i) {
            for (int j = 0; j < ::n; ++j) {
                if (j % CHECK == 0 && timeout()) {
                    goto RET;
                } else {
                    adjust(n, j, rate);
                }
            }
            rate *= 0.999;
        }

        if (n == LIMIT) {
            goto REDO;
        } else {
            continue;
        }
        // partB
        // fprintf(stderr, "%dB: %lf\n", cc, (double)clock() / CLOCKS_PER_SEC);
        if (timeout()) {
            goto RET;
        }
        for (int i = 0; i < m; ++i) {
            rerank(i, m, todo);
        }
        rate = 4.0f;
        for (int i = 0; i < REP; ++i) {
            for (int j = 0; j < m; ++j) {
                if (j % CHECK == 0 && timeout()) {
                    goto RET;
                } else {
                    adjust(m, j, rate);
                }
            }
            rate *= 0.999;
        }

        /*
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < DIMENSION; ++j) {
                p[i][j] = round_(p[i][j]);
            }
        }
        */
    }

RET:
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < DIMENSION; ++j) {
            q[todo[i].second][j] = p[i][j] + .5;
        }
    }
}

struct TheUniverseUnravels {
    vector<string> predictCoordinates(
            const vector<string>& coords,
            const vector<string>& ranks,
            const vector<int>& minDist,
            const vector<int>& maxDist) {
        startTimer();
        n = coords.size();
        for (int i = 0; i < n; ++i) {
            q[i].parse(coords[i]);
            split(ranks[i], rank0[i]);
            mind[i] = sqrt_(minDist[i]);
            maxd[i] = sqrt_(maxDist[i]);
        }
        //::minDist = minDist;
        //::maxDist = maxDist;

        if (n > 1600) {
            LIMIT = 1000;
        } else if (n > 1200) {
            LIMIT = 750;
        } else if (n > 800) {
            LIMIT = 500;
        } else {
            LIMIT = n / 2;
        }
        gao();

        vector<string> ret;
        for (int i = 0; i < n; ++i) {
            ret.push_back(q[i].str());
        }
        return ret;
    }
};

#ifdef __WATASHI__
/**
 * main
 */
vector<string> readLines() {
    static int n;
    static char buf[65536];
    vector<string> ret;

    scanf("%d ", &n);
    for (int i = 0; i < n; ++i) {
        fgets(buf, sizeof(buf), stdin);
        for (int j = 0; buf[j] != '\0'; ++j) {
            if (buf[j] == '\n') {
                buf[j] = '\0';
                break;
            }
        }
        ret.push_back(buf);
    }

    return ret;
}

vector<int> readInts() {
    static int n;
    vector<int> ret;

    scanf("%d", &n);
    ret.resize(n);
    for (int i = 0; i < n; ++i) {
        scanf("%d", &ret[i]);
    }

    return ret;
}

int main() {
    auto coords = readLines();
    auto ranks = readLines();
    auto minDist = readInts();
    auto maxDist = readInts();
    TheUniverseUnravels tuu;
    auto ret = tuu.predictCoordinates(coords, ranks, minDist, maxDist);
    printf("%d\n", (int)ret.size());
    for (auto i: ret) {
        puts(i.c_str());
    }
    fprintf(stderr, "n = %d\n", n);
    fprintf(stderr, "time = %lf\n", (double)clock() / CLOCKS_PER_SEC);
    // fprintf(stderr, "sq = %lf\n", sq);
    fflush(stdout);
    return 0;
}
// vim: ft=cpp.doxygen
#endif
