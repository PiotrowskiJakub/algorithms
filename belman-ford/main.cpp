#include <iostream>
#include <vector>
#include <regex>
#include <fstream>

using namespace std;

class Int {
    int value;
public:
    Int(int value) : value(value) {}

    explicit operator int() const { return value; }

    int val() { return value; }
};

class Edge {
public:
    int from;
    int to;
    int weight;

    Edge(int from, int to, int weight) : from(from), to(to), weight(weight) {}
};

vector<string> split(const string &input, const string &reg) {
    regex re{reg};
    sregex_token_iterator
            first{input.begin(), input.end(), re, -1},
            last;
    return {first, last};
}

vector<vector<Int *>> parseGraphFile(string path) {
    vector<vector<Int *>> matrix;
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
            vector<Int *> row;
            int i = 0;
            for (auto const &value : splitted) {
                if (i == 0) {
                    i++;
                    continue;
                }
                try {
                    row.push_back(new Int(stoi(value)));
                } catch (std::invalid_argument) {
                    row.push_back(NULL);
                }
            }
            matrix.push_back(row);
        }
    }
    return matrix;
}

vector<Edge> buildEdgesModel(const vector<vector<Int *>> &matrix) {
    vector<Edge> edges;
    for (auto i = 0; i < matrix.size(); i++) {
        for (auto j = 0; j < matrix[i].size(); j++) {
            if (i == j || matrix[i][j] == NULL) {
                continue;
            }
            edges.emplace_back(Edge(i, j, (int) *matrix[i][j]));
        }
    }

    return edges;
}

tuple<int *, int *, int> initValues(int start, int size) {
    int *d = new int[size];
    int *p = new int[size];
    int iterations = 0;

    for (int i = 0; i < size; i++) {
        d[i] = 9999;
        p[i] = -1;
    }
    d[start] = 0;
    iterations = 0;

    return make_tuple(d, p, iterations);
}

void printResults(int d[], int p[], int iterations, int size) {
    cout << "Done in " << iterations << " iterations" << endl;
    for (int i = 0; i < size; i++) {
        cout << "d[" << i << "] = \t" << d[i] << "\tp[" << i << "] = \t" << p[i] << endl;
    }
    cout << endl;
}

int executeMatrixBelman(int startPoint, vector<vector<Int *>> &matrix) {
    int *d, *p;
    int iterations;
    tie(d, p, iterations) = initValues(startPoint, matrix.size());

    cout << "Matrix Belman-Ford start point at " << startPoint << endl;
    for (int k = 0; k < matrix.size() - 1; k++) {
        bool test = true;
        for (int i = 0; i < matrix.size(); i++) {
            for (int j = 0; j < matrix.size(); j++) {
                iterations++;
                if (matrix[i][j] == NULL) continue;
                if (d[j] > d[i] + matrix[i][j]->val()) {
                    d[j] = d[i] + matrix[i][j]->val();
                    p[j] = i;
                    test = false;
                }
            }
        }
        if(test) {
            break;
        }
    }
    for (int i = 0; i < matrix.size(); i++) {
        for (int j = 0; j < matrix.size(); j++) {
            iterations++;
            if (matrix[i][j] == NULL) continue;
            if (d[j] > d[i] + matrix[i][j]->val()) {
                cout << "Negative path";
                return 1;
            }
        }
    }

    printResults(d, p, iterations, matrix.size());
    return 0;
}

int executeEdgeBelman(int startPoint, vector<Edge> &edges, int size) {
    int *d, *p;
    int iterations;
    tie(d, p, iterations) = initValues(startPoint, size);

    cout << "Edge Belman-Ford start point at " << startPoint << endl;
    for (int k = 0; k < size - 1; k++) {
        for (auto edge : edges) {
            iterations++;
            int i = edge.from;
            int j = edge.to;
            int w = edge.weight;
            if (d[j] > d[i] + w) {
                d[j] = d[i] + w;
                p[j] = i;
            }
        }
    }
    for (auto edge : edges) {
        iterations++;
        int i = edge.from;
        int j = edge.to;
        int w = edge.weight;
        if (d[j] > d[i] + w) {
            cout << "Negative path";
            return 1;
        }
    }

    printResults(d, p, iterations, size);
    return 0;
}

int main(int argc, char *argv[]) {
    int startPoint = stoi(string(argv[1]));

    vector<vector<Int *>> matrix = parseGraphFile("graph.txt");
    if (startPoint < 0 || startPoint > matrix.size() - 1) {
        cout << "Wrong start point: " << startPoint << endl;
        return 1;
    }
    vector<Edge> edges = buildEdgesModel(matrix);

    executeMatrixBelman(startPoint, matrix);
    executeEdgeBelman(startPoint, edges, matrix.size());

    return 0;
}
