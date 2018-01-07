#include <iostream>
#include <fstream>
#include <map>
#include <queue>

using namespace std;

class Node;

class Comparator {
public:
    bool operator()(const Node *lhs, const Node *rhs);
};

class Node {
public:
    Node *left;
    Node *right;
    char value;
    const long freq;

    Node(const char &value, const long &freq) : value(value), freq(freq), left(nullptr), right(nullptr) {}

    Node(const long &freq, Node *left, Node *right) : value(0), freq(freq), left(left), right(right) {}

    void add(char &value, long &freq, bool &left) {
        Node *node = new Node(value, freq);
        this->add(node, left);
    }

    void add(Node *node, bool &left) {
        if (left) {
            this->left = node;
        } else {
            this->right = node;
        }
    }

    friend ostream &operator<<(ostream &os, const Node &node) {
        os << node.value << "  (" << node.freq << ")";
        return os;
    }

    void save(string filename) {
        ofstream file{filename};
        for (auto &pair : this->traverse()) {
            for (const auto &c : pair.second) {
                file << c;
            }
            file << pair.first;
        }
        file.close();
    }

    static Node *load(string filename) {
        ifstream file{filename};
        Node *root = new Node{0, nullptr, nullptr};
        char c;
        Node *act = root;
        while (file >> noskipws >> c) {
            if (c == '0') {
                if (act->left == nullptr) act->left = new Node{0, nullptr, nullptr};
                act = act->left;
            } else if (c == '1') {
                if (act->right == nullptr) act->right = new Node{0, nullptr, nullptr};
                act = act->right;
            } else {
                act->value = c;
                act = root;
            }
        }
        file.close();
        return root;
    }

    static Node *build(string input) {
        map<char, long> freq;

        for (char &c : input) {
            try {
                freq.at(c)++;
            } catch (out_of_range &) {
                freq[c] = 1;
            }
        }

        vector<Node *> vec;
        for (auto &pair : freq) {
            Node *tmp = new Node(pair.first, pair.second);
            vec.push_back(tmp);
        }
        sort(vec.begin(), vec.end(), Comparator());

        while (vec.size() > 1) {
            Node *top = vec.front();
            vec.erase(vec.begin());
            Node *top2 = vec.front();
            vec.erase(vec.begin());
            Node *tmp = new Node(top->freq + top2->freq, top, top2);
            if (vec.empty()) {
                vec.push_back(tmp);
            } else {
                for (int i = 0; i < vec.size(); i++) {
                    if (tmp->freq <= vec[i]->freq) {
                        vec.insert(vec.begin() + i, tmp);
                        break;
                    } else if (i == vec.size() - 1) {
                        vec.push_back(tmp);
                        break;
                    }
                }
            }
        }

        return vec.front();
    }

    map<char, vector<bool>> traverse() {
        Node *t = this;
        map<char, vector<bool>> paths;
        vector<bool> path;
        traverse(t, path, paths);
        return paths;
    }

    void traverse(Node *node, vector<bool> &path, map<char, vector<bool>> &paths) {
        if (node->left != nullptr) {
            vector<bool> tmp;
            tmp = path;
            tmp.push_back(0);
            traverse(node->left, tmp, paths);
        }
        if (node->right != nullptr) {
            vector<bool> tmp;
            tmp = path;
            tmp.push_back(1);
            traverse(node->right, tmp, paths);
        }
        if (node->left == nullptr && node->right == nullptr) {
            vector<bool> tmp;
            tmp = path;
            paths[node->value] = tmp;
        }
    }
};

vector<bool> compress(string text, map<char, vector<bool>> mapper) {
    vector<bool> ret;
    for (auto &c : text) {
        auto bits = mapper[c];
        ret.insert(ret.end(), bits.begin(), bits.end());
    }
    return ret;
}

string decompress(vector<bool> compressed, Node *root) {
    Node *act = root;
    string a;
    for (const auto &b : compressed) {
        if (act->left == nullptr && act->right == nullptr) {
            a += act->value;
            act = root;
        }
        if (b) act = act->right;
        else act = act->left;
    }
    a += act->value;
    return a;
}

void save(vector<bool> compressed, string filename) {
    ofstream file{filename};
    for (const auto &b : compressed) {
        file << b;
    }
    file.close();
}

void save(string content, string filename) {
    ofstream file{filename};
    file << content;
    file.close();
}

bool Comparator::operator()(const Node *lhs, const Node *rhs) {
    return lhs->freq < rhs->freq;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        cout << "Usage: -c/-d filename -o output_file" << endl;
        return 1;
    }

    string cmd{argv[1]};
    string filename{argv[2]};
    string out_cmd{argv[3]};
    string output_filename{argv[4]};
    ifstream file{filename};

    char c;
    if (cmd == "-c") {
        // Compress
        cout << "Compressing " << filename << endl;
        string input{(std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()};
        Node *root = Node::build(input);
        vector<bool> compressed = compress(input, root->traverse());
        save(compressed, output_filename);
        root->save("tree.txt");
    } else if (cmd == "-d") {
        // Decompress
        cout << "Decompressing " << filename << endl;
        Node *root = Node::load("tree.txt");
        bool b;
        vector<bool> compressed;
        while (file >> noskipws >> c) {
            b = c == '1';
            compressed.push_back(b);
        }
        string content = decompress(compressed, root);
        save(content, output_filename);
    }
    file.close();
    return 0;
}