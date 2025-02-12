// CO21BTECH11002
// Aayush Kumar

#include <bits/stdc++.h>
using namespace std;

vector<string> ciphers = {
    "8cd7172de792e59cb563e4a943c7fd240dfb9c71979245339d75e5a651029e2f9143145c6933f8380ed966ac2809a061d228991b6174cd6d83724b9a79fede30184aa925923dd65507227b8d",
    "80df542bf68be290fc64acaf4281d6002bfb9c6888d750289d75e4bb4a1e9c6c88065b5d623bef381cd375b77b0eb6608024df5a6068cc7bcd6f51d96cefda78185fab238137c3010c3f7ccd5561e46d3213e9240d20b1296487ac56b023a7a8b395cd0a3961e765c2541a0eae",
    "9dd1117fee90fed2f475e5a04493c9453abfdd6cc4c5547b9534ffe945119220c50a150e793eef381bc266f93c0baf718022cc1d7c7485668272028d77aadf314c4ab566953786471c2278834629ec286115f6390772a6666d87e952be6087bfb291851c3161c969c05e1014a518e4dc1ae7356f4e617e7b3b58f05840d3a26bbc68482de5a8b69b7d",
    "88d70d7ff18cf490e27fe3ec4e88de1627b3d96ac4d343328c3debac5719982d8943164b793ee57c1b967bea7b1cb17bc438da137a67857a8c68469675aadf3d5f46b335cc64cf524e227383512efc7a6105aa69016ef2272292bd52ea66c6a5a7d4d6023a6f8b46c35f1b42b657fe9374e02522457a63",
    "9edc532dfbc2e2d5f07ee2ab0d95d90226a39c768bc5112f9034f2e94250962d96105b4b7522e3760bc27de3354ca075ce6ddb1f3463c47d9e6346d97af39b3c4d42a628c026c348002a668d1204e5616801e42c1c68f20d6d8dab56ec77",
    "88cd543ebe88fed2b57ee2b84895c60c2ba0903890d75d37d821eeac4e5082239044094b2d21e37404df7aeb7b18ac34c724cf1f34319438cd76478b7de9de3a4c01e7138e28c3521d6d61cb5761e3677040ef3a4861f2357680bd5aed778fa9a895cb45",
    "87dc023aecc2e1c5e137e3aa4bc7c40c22bb9c6c8bdf5e298a3af1e954189a38c51a145b2d35eb7648d27bac2f04a634c42cc05a7566d16d9f26569675e5c9265758e72c9537d2010f3e35d4572de526",
    "9ed11131beabb1c7f464acad0d8cd90162f7d161c4c250299d3bf2ba031d943a80075b4f2d3ae56c449676f92f4c8a34c121ce1b6d73856e82734c9d38fed3315501e7148f2086650f2372c64027e06d7e04",
    "9dd1117ffd97e3d5b571e3be0d85df172bb3d375c4db427b9b20f4a04c0392389c4d5b7a6533f87d4fc534e2344ca061d228991c7b72856b98744b966be3cf2d160f8329922bd249176d45c2402aec7a",
    "809e197ff58bfddcfc79ebec598edd006ea0d47188d71112d822e7a057509d23974317476b33aa6c079667e4341ba6668020dc5a6369d160cd6b479876e3d533184ea922c02cc7511e247bc64132a7285101ea3f016ef2276c85e97bf16184afb2",
    "80991c3ee887b1ddf07ae3be449dd5016ea3d4719792442f8c30f4a55a508e3f800f1e5d7e76fa710dd571ac340ae37dce2bd6087961d1618268029577e4dc745d41a833872c8655016d65c24132a9693214e33a1c20a3336792bd5af16dc8ea8295c91d3d2f8b6dc253552aaf5af2d649",
    "93dc0630b389ffdfe27be9a84a82900c20a3d96a85d145328e30a6b9511f942adf430c466c22ef6e0dc434f53419e377cf38d51e3463ca659d73569c38e8de32575da266992bd301072361c64020ea7c7704a63e0174ba666f84e952f067c6aba780c0192320d9688c560707e056ffc71ae1392942717f6a7b0c9f1a61daab6cb868432be7afae9660814451",
};

vector<vector<int>> hexToInt(vector<string>& arr) {
    vector<vector<int>> ans(arr.size());
    for(int i=0; i<arr.size(); i++) {
        for(int j=0; j<arr[i].size(); j+=2) {
            string t = arr[i].substr(j, 2);
            ans[i].push_back(stoi(t, 0, 16));
        }
    }
    return ans;
}

void interact(vector<vector<int>> arr) {
    vector<int> key;
    vector<string> msgs(arr.size());
    int mx = 0;
    for(int i=0; i<arr.size(); i++) {
        mx = max(mx, (int)arr[i].size());
    }
    for(int i=0; i<mx; i++) {
        char c = 'n';
        string g;
        int n;
        while(c!='y') {
            cout << "Message Number: " << endl;
            cin >> n;
            cout << "Guess 1 character: " << endl;
            cin.ignore();
            // for inputing space character
            getline(cin, g);
            n--;
            cout << "Messeges: " << endl;
            for(int j=0; j<arr.size(); j++) {
                if(arr[j].size()<=i) {
                    cout << j+1 << " " << msgs[j] << endl;
                    continue;
                }
                if(j!=n) cout << j+1 << " " << msgs[j] << (char)(g[0]^arr[n][i]^arr[j][i]) << endl;
                else cout << j+1 << " " << msgs[j] << g[0] << endl;
            }
            cin >> c;
            if(c=='y') {
                key.push_back(g[0]^arr[n][i]);
                for(int j=0; j<arr.size(); j++) {
                    if(arr[j].size()<=i) continue;
                    if(j!=n) msgs[j] += (g[0]^arr[n][i]^arr[j][i]);
                    else msgs[j] += g[0];
                }
            }
        }
    }
    cout << "Key: ";
    for(int i=0; i<key.size(); i++) {
        cout << key[i] << " ";
    }
    cout << endl;
    cout << "Messages: " << endl;
    for(int i=0; i<arr.size(); i++) {
        cout << msgs[i] << endl;
    }
}

void solve() {
    vector<vector<int>> arr = hexToInt(ciphers);
    interact(arr);
}

int main() {
    ios_base::sync_with_stdio(false); cin.tie(0); cout.tie(0);
    int t=1;
    // cin >> t;
    for(int i=0; i<t; i++) {
        solve();
    }
    return 0;
}