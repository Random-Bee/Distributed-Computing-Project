#include "./include/kademlia.hpp"

int joined = 0;
int ready = 0;
set<int> rem_nodes;

void receive()
{
    while (1)
    {
        zmq::message_t msg;
        zmq::recv_result_t res = receiver.recv(msg, zmq::recv_flags::none);
        string message(static_cast<char *>(msg.data()), msg.size());
        stringstream ss(message);
        string type, sender;
        ss >> type >> sender;
        if (type == "join")
        {
            string reqId;
            ss >> reqId;
            if (rem_nodes.find(stoi(sender)) == rem_nodes.end())
            {
                continue;
            }
            join(reqId, sender);
            // send ack to the sender
            string message = "ack " + to_string(id);
            send(stoi(sender), message);
            rem_nodes.erase(stoi(sender));
        }
        else if (type == "ack")
        {
            joined++;
        }
        else if (type == "ready")
        {
            ready++;
        }
        else if (type == "store")
        {
            string key, value;
            ss >> key >> value;
            route(key, value, sender);
        }
        else if (type == "retrieve")
        {
            string key;
            ss >> key;
            fetch(key, sender);
        }
        else if (type == "fail")
        {
            string key;
            ss >> key;
            cout << "Key: " << key << " not found in " << sender << endl;
        }
        else if (type == "success")
        {
            string key, value;
            ss >> key >> value;
            cout << "Key: " << key << " value: " << value << " found in node: " << sender << endl;
        }
        else if (type == "log")
        {
            string key, value;
            ss >> key >> value;
            table[key] = value;
            cout << "Stored key: " << key << " value: " << value << " in node: " << sender << endl;
        }
        else if (type == "exit")
        {
            break;
        }
    }
}

void wait_for_all()
{
    for (int i = 0; i < n; i++)
    {
        if (i == id)
            continue;
        rem_nodes.insert(i);
    }
    while (joined < n - 1)
    {
        join();
        // wait before sending join request again
        sleep(1);
    }
    string message = "ready " + to_string(n - 1);
    for (int i = 0; i < num_machine; i++)
    {
        send(i, message);
    }
}

void doWork()
{
    cout << "Enter 0 to exit" << endl;
    while (1)
    {
        string choice;
        cin >> choice;
        if (choice == "0")
        {
            string message = "exit " + to_string(id);
            for (int i = 0; i < n; i++)
            {
                send(i, message);
            }
            break;
        }
        else
        {
            cout << "Invalid choice" << endl;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Usage: ./main <machine Number>" << endl;
        return 0;
    }

    int machineNo = stoi(argv[1]);

    int i;

    FILE *file = fopen("inp.txt", "r");

    if (file == NULL)
    {
        cout << "Error in opening the file" << endl;
        return 0;
    }

    fscanf(file, "%d", &n);
    while (1)
    {
        string s;
        char c;
        while (1)
        {
            fscanf(file, "%c", &c);
            if (c == '\n')
                break;
            else if (c == ' ')
                continue;
            s += c;
        }
        if (s.length() != 0){
            s = "tcp://" + s;
            base_endpoint.push_back(s);
        }
        if (feof(file))
            break;
    }
    fclose(file);

    num_machine = base_endpoint.size();
    if (num_machine <= 0)
    {
        cout << "No machine found" << endl;
        return 0;
    }

    for (i = machineNo + num_machine; i <= n - 1; i += num_machine)
    {
        if (fork() == 0)
        {
            id = i;
            break;
        }
        else
        {
            id = machineNo;
        }
    }

    if (id == machineNo)
    {
        cout << "Initializing" << endl;
        init();
        generateNodeId();
        init_tables();
        thread t(receive);
        wait_for_all();
        while (ready < n)
            ;
        cout << "Ready" << endl;
        thread t1(doWork);
        t.join();
        t1.join();
        for (i = machineNo + num_machine; i <= n - 1; i += num_machine)
        {
            wait(NULL);
        }
    }
    else
    {
        init();
        generateNodeId();
        init_tables();
        thread t(receive);
        wait_for_all();
        t.join();
    }

    for (int i = 0; i <= n; i++)
    {
        senders[i].close();
    }
    receiver.close();

    return 0;
}