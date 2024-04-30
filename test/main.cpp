#include "./include/kademlia.hpp"

int joined = 0;
int ready = 0;
set<int> rem_nodes;

string start_time, end_time;

void receive()
{
    while (1)
    {
        zmq::message_t msg;
        zmq::recv_result_t res = receiver.recv(msg, zmq::recv_flags::none);
        string message(static_cast<char *>(msg.data()), msg.size());
        string t_message = message + "\n";
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
            int hops;
            ss >> key >> value >> hops;
            route(key, value, stoi(sender), hops);
        }
        else if (type == "retrieve")
        {
            string key;
            int hops;
            ss >> key >> hops;
            fetch(key, stoi(sender), hops);
        }
        else if (type == "fail")
        {
            string key, hops;
            ss >> key >> hops;
            // cout << "Key: " << key << " not found in " << sender << endl;
        }
        else if (type == "success")
        {
            string key, value, hops;
            ss >> key >> value >> hops;
            end_time = getSysTime();

            // cout << "Key: " << key << " value: " << value << " found in node: " << sender << endl;
        }
        else if (type == "log")
        {
            string key, value, hops;
            ss >> key >> value >> hops;
            table[key] = value;
            end_time = getSysTime();

            // cout << "Stored key: " << key << " value: " << value << " in node: " << sender << endl;
        }
        else if (type == "exit")
        {
            printkeyFile();
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

ifstream test;

void doWork()
{
    test.open("inp-data.txt");
    string s;
    start_time = getSysTime();
    while (getline(test, s))
    {
        // cout << s << "\n";
        if (s.length() == 0)
            break;
        stringstream ss(s);
        int choice;
        ss >> choice;
        // cout << choice;

        if (choice == 1)
        {
            string key, value;
            // cout << "Enter key: ";
            ss >> key;
            // cout << "Enter value: ";
            ss >> value;
            route(key, value, id, 0);
        }
        else if (choice == 2)
        {
            string key;
            // cout << "Enter key: ";
            ss >> key;
            fetch(key, id, 0);
        }
        else if (choice == 3)
        {
            sleep(5);
            string message = "exit " + to_string(id);
            for (int i = 0; i < n; i++)
            {
                send(i, message);
            }
            break;
        }
        // else if (choice == 4)
        // {
        //     printRoutingTable();
        // }
        // else if (choice == 5)
        // {
        //     printCorrMachine();
        // }
        // else if (choice == 6){
        //     printTable();
        // }
        else
        {
            cout << "Invalid choice" << endl;
        }
    }
    printTimeFile(start_time, end_time);
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
        if (s.length() != 0)
        {
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

    cout << num_machine << ":" << machineNo << endl;
    for (i = machineNo + num_machine; i <= n - 1; i += num_machine)
    {
        // cout << i << "," << getAddress(i) << endl;
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
        init();
        generateNodeId();
        init_tables();
        thread t(receive);
        wait_for_all();
        while (ready < n)
            ;
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

    logFile.close();

    return 0;
}