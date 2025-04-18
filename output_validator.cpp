#include <iostream>
#include <vector>
#include <set>
using namespace std;

bool debug = true;

int main() {
    int n = 0, m = 0;
    cin >> n;
    set<pair<int, int> > arcs;
    for (int z = 0; z < n; z++) {
        int left, right;
        cin >> left >> right;
        arcs.insert(make_pair(left, right));
    }
    cin >> m;
    vector<pair<int, int> > mis;
    for (int z = 0; z < m; z++) {
        int left, right;
        cin >> left >> right;
        mis.push_back(make_pair(left, right));
        if (arcs.find(make_pair(left, right)) == arcs.end()) {
            cout << "Infeasible independent set\n";
            if (debug) {
                cout << left << " " << right << endl;
                cout << "27\n";
            }
            return 0;
        }
    }
    for (int z = 0; z < m; z++) {
        if ((
                (mis[(z + m - 1) % m].second >= mis[z].first 
                // || (mis[(z + m - 1) % m].second != 0 && mis[z].first == 0)
                ) 
                && 
                (mis[z].second >= mis[(z + 1) % m].first 
                // || (mis[z].second != 0 && mis[(z + 1) % m].first == 0)
                )
            )
        ) {
            cout << "Infeasible independent set\n";
            if (debug) {
                cout << mis[(z + m - 1) % m].second << " " << mis[z].first << " " << mis[z].second << " " << mis[(z + 1) % m].first << endl;
                cout << (z + 1) % m << endl;
                cout << m << endl;
                cout << "48\n";
            }
            return 0;
        }
    }
    cout << "Independent set is feasible\n";
    return 0;
}
