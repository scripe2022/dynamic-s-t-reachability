// comp := g++ spanning_tree.cpp /home/jyh/.local/include/cpglib/print.o -o spanning_tree -O1 -std=gnu++20 -Wall -Wextra -Wshadow -D_GLIBCXX_ASSERTIONS -fmax-errors=2 -DLOCAL
// run  := ./spanning_tree < data.in
#include <bits/stdc++.h>
using namespace std;
#pragma GCC optimize("unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#ifdef LOCAL
#include <cpglib/print.h>
#define debug(x...) _debug_print(0, #x, x);
#define Debug(x...) _debug_print(1, #x, x);
#define DEBUG(x...) _debug_print(2, #x, x);
std::ifstream terminal("/dev/tty");
#define PP cerr << "![1;30mpause...\e[0m", terminal.ignore();
#else
#define debug(x...)
#define Debug(x...)
#define DEBUG(x...)
#define PP
#endif
template<typename... Args> void print_(Args... args) { ((cout << args << " "), ...) << endl; }
#define rep(i, a, b) for (int i = (a); i < (int)(b); ++i)
#define sz(v) ((int)(v).size())
#define print(...) print_(__VA_ARGS__);
#define FIND(a, x) ((find(a.begin(), a.end(), (x)) != a.end()) ? 1 : 0)
#define cmin(x, ...) x = min({(x), __VA_ARGS__})
#define cmax(x, ...) x = max({(x), __VA_ARGS__})
#define INTMAX (int)(9223372036854775807)
#define INF (int)(1152921504606846976)
#define NaN (int)(0x8b88e1d0595d51d1)
#define double long double
#define int long long
#define uint unsigned long long
#define endl "\n"
#define MAXN 200010

int cnt;
vector<vector<int>> edges;
vector<int> lst, queried;
vector<pair<int, int>> qs;
stack<pair<int, int>> s;

struct LCT {
    vector<int> u, v, parent, tag, id, val;
    vector<vector<int>> ch;
    int n;

    LCT(int N, int M) {
        n = N;
        u.resize(n + M), v.resize(n + M);
        ch.resize(n + M, vector<int>(2));
        parent.resize(n + M);
        tag.resize(n + M);
        id.resize(n + M);
        val.resize(n + M);
    }

    bool isrs(int x) {
        return x == ch[parent[x]][1];
    }

    bool isroot(int x) {
        return x != ch[parent[x]][0] && x != ch[parent[x]][1];
    }

    void pushup(int x) {
        id[x] = val[id[ch[x][0]]] > val[id[ch[x][1]]] ? id[ch[x][0]] : id[ch[x][1]];
        if (val[x] > val[id[x]]) id[x] = x;
    }

    void reverse(int x) {
        tag[x] ^= 1;
        swap(ch[x][0], ch[x][1]);
    }

    void pushdown(int x) {
        if (tag[x]) {
            reverse(ch[x][0]), reverse(ch[x][1]);
            tag[x] = 0;
        }
    }

    void update(int x) {
        if (!isroot(x)) update(parent[x]);
        pushdown(x);
    }

    void rotate(int x) {
        int f = parent[x], g = parent[f], k = isrs(x);
        if (!isroot(f)) ch[g][isrs(f)] = x;
        ch[f][k] = ch[x][k ^ 1];
        if (ch[x][k ^ 1]) parent[ch[x][k ^ 1]] = f;
        ch[x][k ^ 1] = f, parent[x] = g, parent[f] = x;
        pushup(f), pushup(x);
    }

    void splay(int x) {
        update(x);
        for (int f; f = parent[x], !isroot(x); rotate(x)) {
            if (!isroot(f)) rotate(isrs(x) == isrs(f) ? f : x);
        }
    }
    int access(int x) {
        int p;
        for (p = 0; x; p = x, x = parent[x]) splay(x), ch[x][1] = p, pushup(x);
        return p;
    }

    void make_root(int x) {
        access(x);
        splay(x);
        reverse(x);
    }

    int find(int x) {
        access(x);
        splay(x);
        while (ch[x][0]) x = ch[x][0];
        splay(x);
        return x;
    }

    void split(int x, int y) {
        make_root(x);
        access(y);
        splay(y);
    }

    void link(int x, int y) {
        make_root(x);
        if (find(y) != x) {
            parent[x] = y;
        }
    }

    void cut(int x, int y) {
        split(x, y);
        if (find(y) == x && parent[y] == x && !ch[y][0]) {
            parent[y] = ch[x][1] = 0;
            pushup(x);
        }
    }

    void dfs(int l, int r, int p) {
        int top = s.size();
        for (int e: edges[p]) {
            int x = u[e], y = v[e];
            if (find(x) != find(y)) {
                link(x, e), link(y, e);
                s.push({e, 0});
            }
            else {
                split(x, y);
                if (val[id[y]] <= val[e]) continue;
                int t = id[y];
                cut(u[t], t), cut(v[t], t);
                link(x, e), link(y, e);
                s.push({e, t});
            }
        }
        if (l == r) {
            if (queried[l]) {
                auto [x, y] = qs[l];
                if (find(x) != find(y)) cout << "no" << endl;
                else split(x, y), cout << "yes" << endl;
            }
        }
        else {
            int mid = (l + r) >> 1;
            dfs(l, mid, p << 1), dfs(mid + 1, r, p << 1 | 1);
        }
        while ((int)s.size() > top) {
            auto [x, y] = s.top();
            cut(u[x], x), cut(v[x], x);
            if (y) link(u[y], y), link(v[y], y);
            s.pop();
        }
    }

    void add_edge(int x, int y, int i) {
        ++cnt;
        u[n + cnt] = x + 1; v[n + cnt] = y + 1;
        val[n + cnt] = 1;
        lst[cnt] = i;
    }
};

int32_t main() {
    int n, m, q; cin >> n >> m >> q; q += m;
    lst.resize(n+q+1), queried.resize(n+q+1), qs.resize(n+q+1);
    edges.resize((n + q) << 2);
    vector<vector<int>> adj(n+1, vector<int>(n+1, 0));
    LCT lct(n, q);
    for (int i = 1; i <= m; i++) {
        int x, y; cin >> x >> y;
        lct.add_edge(x, y, i);
        adj[x][y] = adj[y][x] = i;
    }
    function<void(int, int, int, int, int, int)> insert = [&](int nl, int nr, int x, int l, int r, int p = 1) {
        if (l > nr || r < nl) return;
        if (nl <= l && r <= nr) {
            edges[p].push_back(x);
            return;
        }
        int mid = (l + r) >> 1;
        insert(nl, nr, x, l, mid, p << 1);
        insert(nl, nr, x, mid + 1, r, p << 1 | 1);
    };

    for (int i = m + 1; i <= q; i++) {
        string line; for (getline(cin, line); line.empty(); getline(cin, line));
        istringstream iss(line);
        string str; iss >> str;
        if (str == "add") {
            int u, v; iss >> u >> v;
            lct.add_edge(u, v, i);
        }
        else if (str == "remove") {
            int u, v; iss >> u >> v;
            int x = adj[u][v];
            insert(lst[x], i, n + x, 1, q, 1);
            lst[x] = 0;
        }
        else if (str == "query") {
            int u, v; iss >> u >> v;
            qs[i] = {u + 1, v + 1};
            queried[i] = 1;
        }
    }
    for (int i = 1; i <= cnt; i++) {
        if (lst[i]) insert(lst[i], q, n + i, 1, q, 1);
    }
    lct.dfs(1, q, 1);

    return 0;
}

