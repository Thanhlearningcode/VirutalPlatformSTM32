#include <bits/stdc++.h>
using namespace std;

struct CAN_Frame {
    uint32_t id{};
    uint8_t dlc{};
    bool extended{};
    vector<uint8_t> data;
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // simple state for two nodes
    unordered_map<int, CAN_Frame> last_rx;
    unordered_map<int, uint32_t> baud;
    baud[1] = 500000; baud[2] = 500000;

    string line;
    while (true) {
        if (!std::getline(cin, line)) break;
        if (line.size() == 0) continue;
        // split by spaces
        stringstream ss(line);
        string cmd; ss >> cmd;
        if (cmd == "CAN_SEND") {
            int src; uint32_t id; int dlc; int ext;
            ss >> src >> id >> dlc >> ext;
            vector<uint8_t> data;
            string byte;
            while (ss >> byte) {
                // accept hex like AA or 0xAA
                try {
                    unsigned int v = 0;
                    if (byte.rfind("0x",0)==0 || byte.rfind("0X",0)==0) {
                        v = stoi(byte, nullptr, 16);
                    } else {
                        v = stoi(byte, nullptr, 16);
                    }
                    data.push_back((uint8_t)(v & 0xFF));
                } catch(...){}
            }
            int dst = (src == 1 ? 2 : 1);
            CAN_Frame f; f.id = id; f.dlc = (uint8_t)dlc; f.extended = (ext!=0); f.data = data;
            last_rx[dst] = f;
            cout << "OK SENT " << src << " " << dst << "\n" << flush;
        } else if (cmd == "CAN_RECV") {
            int node; ss >> node;
            if (last_rx.find(node) == last_rx.end()) {
                cout << "EMPTY\n" << flush;
            } else {
                CAN_Frame &f = last_rx[node];
                cout << "FRAME " << f.id << " " << int(f.dlc) << " " << (f.extended?1:0);
                for (auto b: f.data) {
                    cout << " " << hex << uppercase << setw(2) << setfill('0') << (int)b << dec;
                }
                cout << "\n" << flush;
                last_rx.erase(node);
            }
        } else if (cmd == "CAN_SET") {
            int node; uint32_t b; ss >> node >> b;
            baud[node] = b;
            cout << "OK SET " << node << " " << b << "\n" << flush;
        } else if (cmd == "PING") {
            cout << "PONG\n" << flush;
        } else {
            cout << "ERR Unknown command\n" << flush;
        }
    }
    return 0;
}
