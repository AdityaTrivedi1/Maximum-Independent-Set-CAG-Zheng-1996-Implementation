#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <chrono>
#pragma clang diagnostic ignored "-Wc++11-extensions"
using namespace std;
using namespace std::chrono;

// "Eps" class is made to store indexes of arcs having left or right endpoint at a particular point on circle.
class Eps {
    public:
    vector<int> left;
    vector<int> right;
};

// Used to make custom priority queue
class comparator {
public:
    bool operator() (pair<int, int> a, pair<int, int> b) {
        return a.first > b.first;
    }
};

string filename = "test_cases/1_lac_2.txt";

// "m" is the number of points on the circle.
// "n" is the number of arcs.
// "s" is the number of arcs in "forward_arcs" array
// "t" is the number of arcs in "backward_arcs" array
int m = 0, n = 0, s = 0, t = 0;

// "arcs" array stores the arcs.
vector<vector<int> > arcs;

// "modified_arcs" store the arcs after phase one normalisation.
// any index i, refers to same arc on "arcs" and "modified_arcs" arrays after phase one normalisation.
vector<vector<int> > modified_arcs;

// "forward_arcs" stores index of forward arcs in "modified_arcs" after phase two normalisation.
vector<int> forward_arcs;

// "backward_arcs" stores index of backward arcs in "modified_arcs" after phase two normalisation
vector<int> backward_arcs;

// "next_node" stores the index of child in "forward_arcs" array after the forest of trees are made.
// "next_node" has same length as "forward_arcs" array.
vector<int> next_node;

// "rank_of_node" stores the rank (or informally depth) of each arc in a path in forest.
vector<int> rank_of_node;

// "points" array stores pointers to "Eps" class objects.
vector<Eps*> points;

void phase_one_normalization();
void phase_two_normalization();

// "mergeSort" sorts an array of indexes. These indexes refer to the index of "modified_arcs" array.
void mergeSort(int, int, vector<int>&);
void merge(int, int, int, vector<int>&);

void make_graph();

// This function outputs the maximal independent set starting with the 1st arc in "forward_arcs" array.
// This function uses the "next_node" array to achieve its output.
void outputJ1();

// This function outputs the maximal independent set starting with the i-th arc in "forward_arcs" array.
// This function also append the j-th backward arc to the maximal independent set found from "forward_arcs"
// This function also uses the "next_node" array to achieve its output.
void outputJiAj(int, int);

int main() {
    m = 360;
    cin >> n;
    arcs.assign(n, vector<int>(2, -1));
    points.assign(m, NULL);
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

    auto start = high_resolution_clock::now();

    // 1 (This corresponds to the line numbering of algorithm in research paper)
    // phase one normalisation assigns unique endpoints to all arcs.
    phase_one_normalization();
    // phase two normalisation removes super arcs (arcs which contain other arcs)
    // and sorts and divides the arcs into "forward_arcs" and "backward_arcs" array. 
    phase_two_normalization();

    // 2 (This corresponds to the line numbering of algorithm in research paper)
    // This step makes forest of trees in which any path represents a maximal independent set.
    // representation of this graph is in "next_node" array which stores index of child of every arc in "forward_arcs" array.
    // This graph does not include the backward arcs.
    make_graph();

    // 3 (This corresponds to the line numbering of algorithm in research paper)
    int last = 0, r = 0;
    while (next_node[last] != -1) {
        r++;
        rank_of_node[last] = r;
        last = next_node[last];
    }
    int p = r;

    // 4 (This corresponds to the line numbering of algorithm in research paper)
    int z = 0, y = s;
    while (y - s < t && modified_arcs[backward_arcs[y - s]][0] < modified_arcs[forward_arcs[last]][1]) {
        y++;
    }
    if (y - s == t) {
        outputJ1();
        goto end_of_code;
    }
    while (z < s && modified_arcs[forward_arcs[z]][0] < modified_arcs[forward_arcs[0]][1]) {
        int k = z; r = 0;
        while (next_node[k] != -1 && rank_of_node[k] == INT_MAX) {
            r++;
            rank_of_node[k] = r;
            k = next_node[k];
        }
        if (next_node[k] == -1 && rank_of_node[k] == INT_MAX) {
            r++;
            rank_of_node[k] = r;
            last = k;
            if (r < p) {
                outputJ1();
                goto end_of_code;
            }
            while (y - s < t && modified_arcs[backward_arcs[y - s]][0] < modified_arcs[forward_arcs[last]][1]) {
                y++;
            }
            if (y - s == t) {
                outputJ1();
                goto end_of_code;
            }
            if (modified_arcs[forward_arcs[z]][1] < modified_arcs[backward_arcs[y - s]][0] && modified_arcs[backward_arcs[y - s]][1] < modified_arcs[forward_arcs[z]][0]) {
                outputJiAj(z, y - s);
                goto end_of_code;
            }
        }
        if (rank_of_node[k] != INT_MAX && rank_of_node[k] > r + 1) {
            outputJ1();
            goto end_of_code;
        }
        if (rank_of_node[k] != INT_MAX && rank_of_node[k] == r + 1) {
            if (modified_arcs[forward_arcs[z]][1] < modified_arcs[backward_arcs[y - s]][0] && modified_arcs[backward_arcs[y - s]][1] < modified_arcs[forward_arcs[z]][0]) {
                outputJiAj(z, y - s);
                goto end_of_code;
            }
        }
        z++;
    }
    outputJ1();

    end_of_code:
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Output generated" << endl;
    cout << "Time taken : "
         << duration.count() / 1000.0 << " s" << endl;
    return 0;

}

void phase_one_normalization() {
    // This function assigns unique endpoints to arcs
    // the idea behind achieving this goal is to identify 
    // the points where there are endpoints of more than one arcs
    // suppose at point "z" there are 4 left endpoints and 6 rigth endpoints
    // then that point "z" is decomposed into 10 points on circle with
    // first 4 points given to arcs with left endpoint on point "z"
    // and last 6 points are given to arcs with right endpoints on point "z"

    // We maintain a variable "count" which represents the "shift" we impart to all the points on the circle
    // upon encountering any endpoint of an arc, we modify it by (endpoints + count) and
    // then increment the "count" variable so that all succesive endpoints get shifted.

    // We traverse the "points" array for this computation
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
    for (int z = 0; z < n; z++) {
        if ((modified_arcs[z][0] < modified_arcs[z][1]) && modified_arcs[z][0] != 0) forward_arcs.push_back(z);
        else backward_arcs.push_back(z);
    }


    // 2. sort forward arcs
    mergeSort(0, forward_arcs.size() - 1, forward_arcs);
    // 3. sort backward arcs
    mergeSort(0, backward_arcs.size() - 1, backward_arcs);

    stack<int> st;

    // 4. delete super forward arcs
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
    s = forward_arcs.size();
    
    // 5. delete super bacward arcs
    len = backward_arcs.size();
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
    t = backward_arcs.size();
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

void make_graph() {
    next_node.assign(s, -1);
    rank_of_node.assign(s, INT_MAX);

    // The folowing priority queue stores a "pair" consisting of endpoint and index of arc.
    // our goal is get endpoints in ascending order and index of corresponding arc which
    // we achieve using following data structure.
    priority_queue<pair<int, int>, vector<pair<int, int> >, comparator> ordered_points;
    for (int z = 0; z < s; z++) {
        ordered_points.push(make_pair(modified_arcs[forward_arcs[z]][0], z));
        ordered_points.push(make_pair(modified_arcs[forward_arcs[z]][1], z));
    }
    ordered_points.pop();
    
    queue<int> q;
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

void outputJ1() {
    // We need to output maximal independent set starting with 1st arc in "forward_arcs" array.
    // we initialise "node" with index 0, and start traversing down the path using the "next_node" array and
    // adding all the arcs in the path
    vector<int> mis;
    int node = 0;
    do {
        mis.push_back(forward_arcs[node]);
        node = next_node[node];
    } while (node != -1);

    ofstream fout;
    fout.open(filename, ios::app);
    fout << endl;
    fout << mis.size() << endl;
    for (int z = 0; z < mis.size(); z++) {
        fout << arcs[mis[z]][0] << ' ' << arcs[mis[z]][1] << endl;
    }
    fout.close();
}

void outputJiAj(int z, int y) {
    // We need to output maximal independent set starting with i-th arc in "forward_arcs" array.
    // we initialise "node" with index z, and start traversing down the path using the "next_node" array and
    // adding all the arcs in the path.
    // at end we add j-th arc in "backward_arc"
    vector<int> mis;
    int node = z;
    do {
        mis.push_back(forward_arcs[node]);
        node = next_node[node];
    } while (node != -1);
    mis.push_back(backward_arcs[y]);

    ofstream fout;
    fout.open(filename, ios::app);
    fout << endl;
    fout << mis.size() << endl;
    for (int x = 0; x < mis.size(); x++) {
        fout << arcs[mis[x]][0]  << ' ' << arcs[mis[x]][1] << endl;
    }
    fout.close();
}
