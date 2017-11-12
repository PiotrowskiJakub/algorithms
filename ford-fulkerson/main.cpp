#include<iostream>
#include<vector>
#include <regex>
#include <fstream>
#include <queue>

using namespace std;

class Int {
    int value;
public:
    Int(int value) : value(value) {}

    explicit operator int() const { return value; }

    int val() { return value; }

    void dec(const int &val) {
        this->value -= val;
    }

    void inc(const int &val) {
        this->value += val;
    }
};

vector<string> split(const string &input, const string &reg) {
    // passing -1 as the submatch index parameter performs splitting
    regex re{reg};
    sregex_token_iterator
            first{input.begin(), input.end(), re, -1},
            last;
    return {first, last};
}

vector<vector<Int *>> parseFile(string path) {
    vector<vector<Int *>> ret;
    ifstream file{path};
    if (file.is_open()) {
        string line;
        int c = 0;
        while (getline(file, line)) {
            if (c == 0) {
                c++;
                continue;
            }
            vector<string> splitted = split(line, "\\s+");
            vector<Int *> tmp;
            int i = 0;
            for (auto const &value : splitted) {
                if (i == 0) {
                    i++;
                    continue;
                }
                try {
                    tmp.push_back(new Int(stoi(value)));
                } catch (std::invalid_argument) {
                    tmp.push_back(NULL);
                }
            }
            ret.push_back(tmp);
        }
    } else {
        cout << "No such file!" << endl;
    }
    return ret;
}

class Edge {
public:
    int from;
    int to;
    int act = 0;
    int cap;

    Edge(int from, int to, int cap) : from(from), to(to), cap(cap) {}

    int left() {
        return cap - act;
    }
};

//Two edges are called incident, if they share a vertex.
vector<Edge *> incidentEdges(vector<Edge *> &edges, int v) {
    vector<Edge *> ret;
    for (auto &e : edges) {
        if (e->from == v) {
            ret.push_back(e);
        }
    }

    return ret;
}

void printPath(vector<Edge *> &path) {
    cout << "Path: ";
    for (auto i = 0; i < path.size(); i++) {
        if (i == 0) cout << path[i]->from << " ";
        cout << path[i]->to << " ";
    }
    cout << endl;
}


vector<Edge *> bfs(vector<Edge *> &edges, int s, int d, unsigned long size) {
    bool visited[size];
    memset(visited, 0, size * sizeof(int));
    queue<int> q;
    q.push(s);
    Edge *parent[size];
    memset(parent, 0, size * sizeof(Edge *));
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (auto e : incidentEdges(edges, u)) {
            if (!visited[e->to] && e->left() > 0) {
                q.push(e->to);
                parent[e->to] = e;
                visited[e->to] = true;
            }
        }
    }
    vector<Edge *> path;
    if (visited[d]) {
        Edge *e = parent[d];
        while (e != NULL) {
            path.insert(path.begin(), e);
            e = parent[e->from];
        }
    }
    return path;
}

int minLeft(vector<Edge *> &path) {
    int min = INT32_MAX;
    for (auto e: path) {
        if (e->left() < min) {
            min = e->left();
        }
    }
    return min;
}

Edge *getOrAddEdge(vector<Edge *> &edges, int fro, int to, int cap = 0) {
    Edge *tmp = NULL;
    for (auto e : edges) {
        if (e->from == fro && e->to == to) {
            return e;
        }
    }
    tmp = new Edge{fro, to, cap};
    tmp->act = cap;
    return tmp;
}

int algoEdges(vector<Edge *> &edges, int start, int end, unsigned long size) {
    vector<Edge *> path;
    int flow = 0;
    while ((path = bfs(edges, start, end, size)).size() > 0) {
        printPath(path);
        int min = minLeft(path);
        if (min > 0) {
            flow += min;
            for (auto edge : path) {
                edge->act += min;
                Edge *rev = getOrAddEdge(edges, edge->to, edge->from, min);
                rev->act -= min;
            }
        }
    }
    return flow;
}

bool bfs(vector<vector<Int *>> &graph, int s, int d, int parent[]) {
    bool visited[graph.size()];
    memset(visited, 0, graph.size() * sizeof(bool));
    queue<int> q;
    q.push(s);
    parent[s] = -1;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (auto v = 0; v < graph[u].size(); v++) {
            if (graph[u][v] == NULL) {
                continue;
            }
            if (!visited[v] && graph[u][v]->val() > 0) {
                q.push(v);
                parent[v] = u;
                visited[v] = true;
            }
        }
    }
    return visited[d];
}

int minLeft(vector<vector<Int *>> &graph, int parent[], int s, int d) {
    int min = INT32_MAX;
    for (auto i = d; i != s; i = parent[i]) {
        int from = parent[i];
        if (graph[from][i]->val() < min) {
            min = graph[from][i]->val();
        }
    }
    return min;
}

void printPath(int parent[], int start, int end) {
    vector<int> tmp;
    for (auto i = end; i != start; i = parent[i]) {
        tmp.insert(tmp.begin(), i);
    }
    tmp.insert(tmp.begin(), start);
    cout << "Path: ";
    for (auto i : tmp) {
        cout << i << " ";
    }
    cout << endl;
}

int algoMatrix(vector<vector<Int *>> &graph, int start, int end) {
    int parent[graph.size()];
    int flow = 0;
    while (bfs(graph, start, end, parent)) {
        printPath(parent, start, end);
        int min = minLeft(graph, parent, start, end);
        if (min > 0) {
            flow += min;
            for (auto i = end; i != start; i = parent[i]) {
                int from = parent[i];
                graph[from][i]->dec(min);
                if (graph[i][from] == NULL) {
                    graph[i][from] = new Int(0);
                }
                graph[i][from]->inc(min);
            }
        }

    }
    return flow;
}

int main(int argc, char *argv[]) {
    int start = stoi(string(argv[1]));
    int end = stoi(string(argv[2]));
    vector<vector<Int *>> matrix = parseFile("graph.txt");
    vector<Edge *> edges;
    for (auto i = 0; i < matrix.size(); i++) {
        for (auto j = 0; j < matrix[i].size(); j++) {
            if (i == j || matrix[i][j] == NULL) {
                continue;
            }
            edges.push_back(new Edge(i, j, (int) *matrix[i][j]));
        }
    }

    cout << "Flow: " << algoEdges(edges, start, end, matrix.size()) << endl;
    cout << "Flow: " << algoMatrix(matrix, start, end) << endl;

    return 0;
}