// Pastry DHT

#include <bits/stdc++.h>
#include <sys/wait.h>
#include <zmq.hpp>
#include <semaphore.h>
#include <sys/time.h>
using namespace std;

const string base_endpoint = "tcp://127.0.0.1:";

int n, b = 4, base = 16, id;
string nodeId;

// List of sockets for sending messages to other processes
zmq::socket_t* senders;

// Socket for receiving messages from other processes
zmq::socket_t receiver;

// Context for zmq
zmq::context_t context;

// Initialize the sockets and process variables
void init() {
    // initialize sockets
    senders = new zmq::socket_t[n+1];
    context = zmq::context_t(1);
    receiver = zmq::socket_t(context, ZMQ_PULL);
    receiver.bind(base_endpoint + to_string(5555 + id));
    // 0 to n-1 processes are workers
    // n-th process is parent process, it will send completion message to all processes
    for(int i=0; i<=n; i++) {
        senders[i] = zmq::socket_t(context, ZMQ_PUSH);
        senders[i].connect(base_endpoint + to_string(5555 + i));
    }
}

// log function to calculate log base y of x
int log(int x, int y) {
    int ans = 0, c = 1;
    while(c < x) {
        c *= y;
        ans++;
    }
    return ans;
}

vector<bool> convertHexToByte(string hex) {
    vector<bool> byte;
    for(int i = 0; i < hex.size(); i++) {
        if(hex[i] >= '0' && hex[i] <= '9') {
            int val = hex[i] - '0';
            for(int j = 0; j < 4; j++) {
                byte.push_back(val % 2);
                val /= 2;
            }
        } else {
            int val = hex[i] - 'a' + 10;
            for(int j = 0; j < 4; j++) {
                byte.push_back(val % 2);
                val /= 2;
            }
        }
    }
    return byte;
}

string convertByteToHex(vector<bool> byte) {
    string hex = "";
    for(int i = 0; i < byte.size(); i += 4) {
        int val = 0;
        for(int j = 0; j < 4; j++) {
            val += byte[i + j] * pow(2, j);
        }
        if(val < 10) {
            hex += (char)(val + '0');
        } else {
            hex += (char)(val - 10 + 'a');
        }
    }
    return hex;
}

// distance function to calculate distance between two nodes
__int128_t distance(string a, string b) {
    vector<bool> byteA = convertHexToByte(a);
    vector<bool> byteB = convertHexToByte(b);
    __int128_t dist = 0;
    for(int i = 0; i < byteA.size(); i++) {
        dist = dist * 2 + (byteA[i] ^ byteB[i]);
    }
    return dist;
}

void generateNodeId() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long int micros = tv.tv_sec * 1000000 + tv.tv_usec;
    srand(micros);
    string hex = "";
    for(int i = 0; i < 32; i++) {
        hex += (char)(rand() % 16);
    }
    nodeId = convertByteToHex(convertHexToByte(hex));
}

vector<vector<string>> routing_table;
vector<vector<int>> corr_machine;
vector<vector<pair<string,int>>> leaf_set;

int joinCount = 0;

void receive() {
    while(1) {
        zmq::message_t msg;
        zmq::recv_result_t res = receiver.recv(msg, zmq::recv_flags::none);
        string message(static_cast<char*>(msg.data()), msg.size());
        stringstream ss(message);
        string type, sender;
        ss >> type >> sender;
        if(type == "join") {
            joinCount++;
            string reqId;
            ss >> reqId;
            // add node to routing table if possible
            int comPre = 0;
            for(int i = 0; i < nodeId.size(); i++) {
                if(nodeId[i] == reqId[i]) {
                    comPre++;
                } else {
                    break;
                }
            }
            if(comPre < log(n, base)) {
                int row = comPre;
                int col;
                if(reqId[comPre] >= '0' && reqId[comPre] <= '9') {
                    col = reqId[comPre] - '0';
                } else {
                    col = reqId[comPre] - 'a' + 10;
                }
                routing_table[row][col] = reqId;
                corr_machine[row][col] = stoi(sender);
            }
            // add node to leaf set if possible
            if(reqId < nodeId) {
                leaf_set[0].push_back({reqId, stoi(sender)});
            } else {
                leaf_set[1].push_back({reqId, stoi(sender)});
            }
            sort(leaf_set[0].begin(), leaf_set[0].end());
            sort(leaf_set[1].begin(), leaf_set[1].end());
            if(leaf_set[0].size() > base) {
                leaf_set[0].pop_back();
            }
            if(leaf_set[1].size() > base) {
                leaf_set[1].erase(leaf_set[1].begin());
            }
            if(joinCount == n-1) {
                break;
            }
        }
    }
}

void send(int dest, string message) {
    zmq::message_t msg(message.size());
    memcpy(msg.data(), message.c_str(), message.size());
    senders[dest].send(msg, zmq::send_flags::none);
}

void init_tables() {
    routing_table.resize(log(n, base), vector<string>(base, ""));
    corr_machine.resize(log(n, base), vector<int>(base, -1));
    leaf_set.resize(2);
}

void join() {
    string message = "join " + to_string(id) + " " + nodeId;
    for(int i=0; i<n; i++) {
        if(i == id) continue;
        send(i, message);
    }
}

int main() {
    int i;

    FILE* file = fopen("inp.txt", "r");

    if(file == NULL) {
        cout << "Error in opening the file" << endl;
        return 0;
    }

    fscanf(file, "%d", &n);

    fclose(file);

    for(i = 0; i < n; i++) {
        if(fork() == 0) {
            break;
        }
    }

    id = i;

    if(i==n) {
        init();
        // cout << "Choose function" << endl;
        // cout << "1. Store Data" << endl;
        // cout << "2. Retrieve Data" << endl;
        // cout << "3. Exit" << endl;
        // int choice;
        // cin >> choice;

        for(i=0; i<n; i++) {
            wait(NULL);
        }
        cout << "All processes are ready" << endl;
    }
    else {
        init();
        generateNodeId();
        init_tables();
        thread t(receive);
        sleep(1);
        join();
        t.join();

        if(id==0) {
            cout << "Node ID: " << nodeId << " " << id << endl;
            cout << "Routing Table: " << endl;
            for(int i = 0; i < routing_table.size(); i++) {
                for(int j = 0; j < routing_table[i].size(); j++) {
                    cout << routing_table[i][j] << " ";
                }
                cout << endl;
            }
            cout << "Leaf Set: " << endl;
            for(int i = 0; i < leaf_set.size(); i++) {
                for(int j = 0; j < leaf_set[i].size(); j++) {
                    cout << leaf_set[i][j].first << " ";
                }
                cout << endl;
            }
        }
    }

    for(int i=0; i<=n; i++) {
        senders[i].close();
    }
    receiver.close();

    return 0;
}