#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
// #include "Eps.h"
using namespace std;

class Eps {
    public:
    vector<int> left;
    vector<int> right;
};

class comparator {
public:
    bool operator() (pair<int, int> a, pair<int, int> b) {
        return a.first > b.first;
    }
};

int m = 0, n = 0, s = 0;
vector<vector<int> > arcs;
vector<int> forward_arcs;
vector<int> backward_arcs;
vector<int> next_node;
vector<int> rank_of_node;
vector<Eps*> points;
vector<vector<int> > modified_arcs;
string filename = "testcase.txt";

void phase_one_normalization();
void phase_two_normalization();
void mergeSort(int, int, vector<int>&);
void merge(int, int, int, vector<int>&);
void printModifiedArcs();
void printNormalized();
void make_graph();
void printGraph();

int main() {
    m = 360;
    cin >> n;
    arcs.assign(n, vector<int>(2, -1));
    points.assign(m + 1, NULL);
    modified_arcs.assign(n, vector<int>(2, -1));
    for (int z = 0; z < n; z++) {
        int l, r;
        cin >> l >> r;
        arcs[z][0] = l;
        arcs[z][1] = r;
        if (points[l] == NULL) points[l] = new Eps();
        if (points[r] == NULL) points[r] = new Eps();
        points[l] -> left.push_back(z);
        points[r] -> right.push_back(z);
    }

    phase_one_normalization();
    phase_two_normalization();
    printModifiedArcs();
    printNormalized();

    make_graph();
    printGraph();

    
}

void phase_one_normalization() {
    int count = 0;
    for (int z = 1; z <= m; z++) {
        if (points[z % m] == NULL) continue;
        int len = points[z % m] -> left.size();
        for (int y = 0; y < len; y++) {
            int ind = points[z % m] -> left[y];
            modified_arcs[ind][0] = arcs[ind][0] + count + (z == m ? m : 0);
            count++;
        }
        len = points[z % m] -> right.size();
        for (int y = 0; y < len; y++) {
            int ind = points[z % m] -> right[y];
            modified_arcs[ind][1] = arcs[ind][1] + count + (z == m ? m : 0);
            count++;
        }
    }
}


void phase_two_normalization() {
    // 1. separate backward and forward arcs
    // 2. sort forward arcs
    // 3. sort backward arcs
    // 4. delete super forward arcs
    // 5. delete super bacward arcs
    for (int z = 0; z < n; z++) {
        if ((modified_arcs[z][0] < modified_arcs[z][1]) && modified_arcs[z][0] != 0) forward_arcs.push_back(z);
        else backward_arcs.push_back(z);
    }
    mergeSort(0, forward_arcs.size() - 1, forward_arcs);
    mergeSort(0, backward_arcs.size() - 1, backward_arcs);
    stack<int> st;
    int len = forward_arcs.size();
    for (int z = 0; z < len; z++) {
        while (!st.empty() && modified_arcs[st.top()][1] > modified_arcs[forward_arcs[z]][1]) {
            st.pop();
        }
        st.push(forward_arcs[z]);
    }
    forward_arcs.clear();
    while (!st.empty()) {
        forward_arcs.push_back(st.top());
        st.pop();
    }
    reverse(forward_arcs.begin(), forward_arcs.end());
    len = backward_arcs.size();
    s = forward_arcs.size();

    for (int z = 0; z < len; z++) {
        while (!st.empty() && modified_arcs[st.top()][1] > modified_arcs[backward_arcs[z]][1]) {
            st.pop();
        }
        st.push(backward_arcs[z]);
    }
    backward_arcs.clear();
    while (!st.empty()) {
        backward_arcs.push_back(st.top());
        st.pop();
    }
    reverse(backward_arcs.begin(), backward_arcs.end());
}

void mergeSort(int low, int high, vector<int>& arr) {
    if (low < high) {
        int mid = low + (high - low) / 2;
        mergeSort(low, mid, arr);
        mergeSort(mid + 1, high, arr);
        merge(low, mid, high, arr);
    }
}

void merge(int low, int mid, int high, vector<int>& arr) {
    int z = low, y = mid + 1, x = high;
    vector<int> brr;
    while (z <= mid && y <= high) {
        if (modified_arcs[arr[z]][0] < modified_arcs[arr[y]][0]) {
            brr.push_back(arr[z]);
            z++;
        } else {
            brr.push_back((arr[y]));
            y++;
        }
    }
    while (z <= mid) {
        brr.push_back(arr[z]);
        z++;
    }
    while (y <= high) {
        brr.push_back((arr[y]));
        y++;
    }
    z = low, x = 0;
    while (z <= high) {
        arr[z++] = brr[x++];
    }
}

void printModifiedArcs() {
    ofstream fout;
    fout.open(filename, ios::app);
    fout << endl;
    for (int z = 0; z < n; z++) {
        fout << modified_arcs[z][0] << " " << modified_arcs[z][1] << endl;
    }
    fout.close();
}

void printNormalized() {
    ofstream fout;
    fout.open(filename, ios::app);
    fout << endl;
    fout << "Forward arcs size: " << forward_arcs.size() << endl;
    for (int z = 0; z < forward_arcs.size(); z++) {
        fout << forward_arcs[z] << endl;
    }
    fout << "Backward arcs size: " << backward_arcs.size() << endl;
    for (int z = 0; z < backward_arcs.size(); z++) {
        fout << backward_arcs[z] << endl;
    }
    fout.close();
}

void make_graph() {
    next_node.assign(s, -1);
    rank_of_node.assign(s, INT_MAX);

    queue<int> q;
    priority_queue<pair<int, int>, vector<pair<int, int> >, comparator> ordered_points;
    for (int z = 0; z < s; z++) {
        ordered_points.push(make_pair(modified_arcs[forward_arcs[z]][0], z));
        ordered_points.push(make_pair(modified_arcs[forward_arcs[z]][1], z));
    }
    ordered_points.pop();
    
    q.push(0);
    for (int z = 1; z < 2 * s; z++) {
        pair<int, int> temp = ordered_points.top();
        ordered_points.pop();

        if (modified_arcs[forward_arcs[temp.second]][0] != temp.first) continue;
        while (!q.empty() && modified_arcs[forward_arcs[q.front()]][1] < temp.first) {
            int first = q.front();
            q.pop();
            next_node[first] = temp.second;
        }
        next_node[temp.second] = -1;
        rank_of_node[temp.second] = INT_MAX;
        q.push(temp.second);
    }
}

void printGraph() {
    ofstream fout;
    fout.open(filename, ios::app);
    fout << endl;
    fout << "NEXT array: " << endl;
    for (int z = 0; z < s; z++) {
        fout << forward_arcs[z] << " -> " << ((next_node[z] == -1) ? -1 : forward_arcs[next_node[z]]) << endl;
    }
    fout.close();
}
