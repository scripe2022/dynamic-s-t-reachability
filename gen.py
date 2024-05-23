# run := python3 gen.py
import random

def get_pair(n, ordered=True):
    a = random.randint(0, n-1)
    b = random.randint(0, n-2)
    if b >= a:
        if b == a:
            b += 1
    return ((a, b) if a < b else (b, a)) if ordered else (a, b)

def gen_graph(n, m):
    edges = set()
    while len(edges) < m:
        u, v = get_pair(n)
        edges.add((u, v))
    return edges

def get_ans(n, edges, s, t):
    e = [set() for _ in range(n)]
    for u, v in edges:
        e[u].add(v)
        e[v].add(u)
    visited = [False] * n

    def dfs(u):
        visited[u] = True
        for v in e[u]:
            if not visited[v]:
                dfs(v)

    dfs(s)
    return visited[t]

N = 100
M = 150
Q = 100
P = 0.4 # probability of query
edges = gen_graph(N, M)
with open("data.in", "w") as f:
    f.write(f"{N} {M} {Q}\n")
    for u, v in edges:
        f.write(f"{u} {v}\n")
    for _ in range(Q):
        op = "remove" if random.random() > P else "query"
        if op == "remove":
            e = random.choice(list(edges))
            edges.remove(e)
            a, b = e
        else:
            a, b = get_pair(N, False)
            print("yes" if get_ans(N, edges, a, b) else "no")
        f.write(f"{op} {a} {b}\n")
