#include "utils.hpp"

int numNodesStored = 128;
int k = 1;
// Storing routing table as a 128 size vector of queues. Each queue stores max k nodes
vector<queue<string>> routing_table;
vector<queue<int>> corr_machine;

// Initialize routing table
void init_tables()
{
    routing_table.resize(128, queue<string>());
    corr_machine.resize(128, queue<int>());
}

// First bit that is different in two nodes
unsigned int commonPrefix(string h1, string h2)
{
    vector<bool> byte1 = convertHexToByte(h1);
    vector<bool> byte2 = convertHexToByte(h2);
    if (byte1.size() != byte2.size())
    {
        return 0;
    }
    for (int i = 0; i < byte1.size(); i++)
    {
        if (byte1[i] != byte2[i])
        {
            return i;
        }
    }
    return byte1.size();
}

// distance function to calculate distance between two nodes
unsigned __int128 distance(string a, string b)
{
    vector<bool> byteA = convertHexToByte(a);
    vector<bool> byteB = convertHexToByte(b);
    // Kadmelia distance calculation
    // XOR of two nodes
    unsigned __int128 x = 0;
    for (int i = 0; i < byteA.size(); i++)
    {
        x = x * 2 + (byteA[i] ^ byteB[i]);
    }
    return x;
}

int minDistanceNode(string hash)
{
    unsigned __int128 minDist = distance(hash, nodeId);
    int corrMachine = id;
    for (int i = 0; i < numNodesStored; i++)
    {
        if (routing_table[i].size() > 0)
        {
            unsigned __int128 dist = distance(routing_table[i].front(), hash);
            if (dist < minDist)
            {
                minDist = dist;
                corrMachine = corr_machine[i].front();
            }
        }
    }
    return corrMachine;
}

void route(string key, string value, int sender)
{
    string hash = getHash(key);
    vector<bool> byte = convertHexToByte(hash);
    // Check if key can be routed to some node in the routing table
    // For kadmelia, we need to find the node with the closest distance to the key
    int minDistNodeId = minDistanceNode(hash);
    if (minDistNodeId != id)
    {
        string message = "store " + to_string(sender) + " " + key + " " + value;
        send(minDistNodeId, message);
    }
    else
    {
        table[key] = value;
        string message = "log " + to_string(id) + " " + key + " " + value;
        send(sender, message);
    }
}

// Fetch value of a key from the network
void fetch(string key, int sender)
{
    string hash = getHash(key);
    vector<bool> byte = convertHexToByte(hash);
    // Check if key can be routed to some node in the routing table
    // For kadmelia, we need to find the node with the closest distance to the key
    int minDistNode = minDistanceNode(hash);
    if (minDistNode != id)
    {
        // Send the key to the node with the closest distance
        string message = "retrieve " + to_string(id) + " " + key;
        send(minDistNode, message);
    }
    else
    {
        // Check for the key in the current node
        for (auto x : table)
        {
            cout << x.first << " " << x.second << endl;
        }
        if (table.find(key) == table.end())
        {
            string message = "fail " + to_string(id) + " " + key;
            send(sender, message);
        }
        else
        {
            string message = "success " + to_string(id) + " " + key + " " + table[key];
            send(sender, message);
        }
    }
}

// Receive a join request from a new node
void join(string reqId, string sender)
{
    // Find the common prefix between the new node and the current node
    int comPre = commonPrefix(reqId, nodeId);
    // Check if size of the queue at the common prefix is less than k
    if (routing_table[comPre].size() < k)
    {
        routing_table[comPre].push(reqId);
        corr_machine[comPre].push(stoi(sender));
    }
    else
    {
        // Replace the first element in the queue with the new node for now
        routing_table[comPre].pop();
        routing_table[comPre].push(reqId);
        corr_machine[comPre].pop();
        corr_machine[comPre].push(stoi(sender));
        // Replace this logic with the actual kadmelia routing logic
    }
}

void printRoutingTable()
{
    for (int i = 0; i < numNodesStored; i++)
    {
        if (routing_table[i].size() > 0)
        {
            cout << "Routing table at " << i << ": " << routing_table[i].front() << endl;
        }
    }
}

void printCorrMachine()
{
    for (int i = 0; i < numNodesStored; i++)
    {
        if (corr_machine[i].size() > 0)
        {
            cout << "Corresponding machine at " << i << ": " << corr_machine[i].front() << endl;
        }
    }
}