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

    void subtract(const int &val) {
        this->value -= val;
    }

    void add(const int &val) {
        this->value += val;
    }
};

class Edge {
public:
    int from;
    int to;
    int cap;
    int act = 0;

    Edge(int from, int to, int cap) : from(from), to(to), cap(cap) {}

    int left() {
        return cap - act;
    }
};

vector<string> split(const string &input, const string &reg) {
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


// Matrix implementation

void printPath(int parent[], int start, int end) {
    vector<int> path;
    for (auto i = end; i != start; i = parent[i]) {
        path.insert(path.begin(), i);
    }
    path.insert(path.begin(), start);
    cout << "Path: ";
    for (auto i : path) {
        cout << i << " ";
    }
    cout << endl;
}

int minLeft(vector<vector<Int *>> &graph, int parent[], int source, int sink) {
    int min = INT32_MAX;
    for (auto i = sink; i != source; i = parent[i]) {
        int from = parent[i];
        if (graph[from][i]->val() < min) {
            min = graph[from][i]->val();
        }
    }
    return min;
}

bool bfs(vector<vector<Int *>> &matrix, int source, int sink, int parent[]) {
    bool visited[matrix.size()];
    memset(visited, 0, matrix.size() * sizeof(bool));
    queue<int> queue;
    queue.push(source);
    parent[source] = -1;
    while (!queue.empty()) {
        int node = queue.front();
        queue.pop();
        for (auto v = 0; v < matrix[node].size(); v++) {
            if (matrix[node][v] == NULL) {
                continue;
            }
            if (!visited[v] && matrix[node][v]->val() > 0) {
                queue.push(v);
                parent[v] = node;
                visited[v] = true;
            }
        }
    }
    return visited[sink];
}

void matrixVersion(vector<vector<Int *>> &matrix, int source, int sink) {
    int parent[matrix.size()];
    int flow = 0;

    while (bfs(matrix, source, sink, parent)) {
        printPath(parent, source, sink);
        int min = minLeft(matrix, parent, source, sink);
        if (min > 0) {
            flow += min;
            for (auto i = sink; i != source; i = parent[i]) {
                int from = parent[i];
                matrix[from][i]->subtract(min);
                if (matrix[i][from] == NULL) {
                    matrix[i][from] = new Int(0);
                }
                matrix[i][from]->add(min);
            }
        }

    }

    cout << "Maximum flow: " << flow << endl;
}


// Edges implementation

void printPath(vector<Edge *> &path) {
    cout << "Path: ";
    for (auto i = 0; i < path.size(); i++) {
        if (i == 0) cout << path[i]->from << " ";
        cout << path[i]->to << " ";
    }
    cout << endl;
}

vector<Edge *> initEdges(const vector<vector<Int *>> &matrix) {
    vector<Edge *> edges;

    for (auto i = 0; i < matrix.size(); i++) {
        for (auto j = 0; j < matrix[i].size(); j++) {
            if (i == j || matrix[i][j] == NULL) {
                continue;
            }
            edges.push_back(new Edge(i, j, (int) *matrix[i][j]));
        }
    }

    return edges;
}

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

int minLeft(vector<Edge *> &path) {
    int min = INT32_MAX;
    for (auto e: path) {
        if (e->left() < min) {
            min = e->left();
        }
    }
    return min;
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

void edgesVersion(vector<Edge *> &edges, int start, int end, unsigned long size) {
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

    cout << "Maximum flow: " << flow << endl;
}

int main(int argc, char *argv[]) {
    int source = stoi(string(argv[1]));
    int sink = stoi(string(argv[2]));

    vector<vector<Int *>> matrix = parseFile("graph.txt");
    vector<Edge *> edges = initEdges(matrix);

    cout << "\nMatrix:" << endl;
    matrixVersion(matrix, source, sink);

    cout << "\nEdges:" << endl;
    edgesVersion(edges, source, sink, matrix.size());

    return 0;
}