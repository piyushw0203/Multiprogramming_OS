#include <bits/stdc++.h>
using namespace std;

class Cpu
{
private:
    char IR[4];
    bool C;
    int IC = 0;
    char R[4];
    int SI;

public:
    Cpu()
    {
        fill(begin(IR), end(IR), '0');
        C = false;
        IC = 0;
        fill(begin(R), end(R), '0');
    }

    char *getIR()
    {
        return IR;
    }

    char getIR(int index)
    {
        return IR[index];
    }

    void setIR(char newIR[4])
    {
        memcpy(IR, newIR, sizeof(IR));
    }

    bool getC()
    {
        return C;
    }

    void setC(bool newC)
    {
        C = newC;
    }

    int getIC()
    {
        return IC;
    }

    void setIC(int newIC)
    {
        IC = newIC;
    }

    char *getR()
    {
        return R;
    }

    void setR(char newR[4])
    {
        memcpy(R, newR, sizeof(R));
    }

    int getSI()
    {
        return SI;
    }

    void setSI(int newSI)
    {
        SI = newSI;
    }

    string getOpcode()
    {
        string opcode = "";
        opcode += IR[0];
        if (opcode == "H")
        {
            return opcode;
        }
        opcode += IR[1];
        return opcode;
    }

    int getOperand()
    {
        return stoi(string(1, IR[2]) + string(1, IR[3]));
    }
};

class MainMemory
{
private:
    char M[100][4];

public:
    MainMemory()
    {
        fill(&M[0][0], &M[0][0] + sizeof(M), 0);
    }

    char (*getMemory())[4]
    {
        return M;
    }

    void setMemory(char newM[100][4])
    {
        memcpy(M, newM, sizeof(M));
    }
};

class SPARS
{
private:
    MainMemory M;
    Cpu cpu;
    ifstream input;
    ofstream output;
    char buffer[40];
    int used_memory = 0;

public:
    SPARS(const string &inputFileName, const string &outputFileName)
    {
        try
        {
            input.open(inputFileName);
            output.open(outputFileName);
        }
        catch (const exception &e)
        {
            cout << "Error in initializing the input and output file" << endl;
        }
    }

    void init()
    {
        cout << "Initializing the main memory" << endl;
        used_memory = 0;
        M = MainMemory();
        cpu = Cpu();
    }

    void LOAD()
    {
        string line = "";

        try
        {
            while (getline(input, line))
            {
                strncpy(buffer, line.c_str(), sizeof(buffer));

                if (buffer[0] == '$' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'J')
                {
                    cout << "Program card detected " << buffer[4] << buffer[5] << buffer[6] << buffer[7] << endl;
                    init();
                    continue;
                }
                else if (buffer[0] == '$' && buffer[1] == 'D' && buffer[2] == 'T' && buffer[3] == 'A')
                {
                    cout << "Data card detected" << endl;
                    startExecution();
                    continue;
                }
                else if (buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D')
                {
                    cout << "End card detected" << endl;
                    printMemory();
                    continue;
                }

                if (used_memory == 100)
                {
                    cout << "Memory is full" << endl;
                }

                cout << "Loading the instructions to memory" << endl;
                char(*memory)[4] = M.getMemory();

                for (int i = 0; i < line.length();)
                {
                    memory[used_memory][i % 4] = buffer[i];
                    if (buffer[i] == 'H' || buffer[i] == '\0')
                    {
                        used_memory += 10 - (used_memory % 10);
                    }

                    i++;
                    if (i % 4 == 0)
                    {
                        used_memory++;
                    }
                }

                if (used_memory % 10 == 9)
                    used_memory += 10 - (used_memory % 10);

                M.setMemory(memory);
                printMemory();
            }
        }
        catch (const out_of_range &e)
        {
            cout << "Memory is full" << endl;
        }
        catch (const exception &e)
        {
            cerr << e.what() << endl;
        }
    }

    void startExecution()
    {
        cout << "Starting the execution" << endl;
        char(*memory)[4] = M.getMemory();
        cpu.setIC(0);
        executeUserProgram(memory);
    }

    void executeUserProgram(char (*memory)[4])
    {
        while (true)
        {
            memcpy(cpu.getIR(), &memory[cpu.getIC()], sizeof(char) * 4);
            cpu.setIC(cpu.getIC() + 1);

            string opcode = cpu.getOpcode();
            cout << "The opcode is: " << opcode << endl;

            if (opcode == "H")
            {
                cpu.setSI(3);
                M.setMemory(memory);
                MOS();
                return;
            }

            switch (opcode[0])
            {
            case 'L':
            {
                cpu.setR(&memory[cpu.getOperand()][0]);
            }
            break;
            case 'S':
            {
                cout << "The operand is: " << cpu.getOperand() << endl;
                char *arr = cpu.getR();
                for (int i = 0; i < 4; i++)
                {
                    cout << arr[i] << endl;
                }
                memcpy(&memory[cpu.getOperand()], arr, sizeof(char) * 4);
            }
            break;
            case 'C':
            {
                if (memcmp(cpu.getR(), &memory[cpu.getOperand()], sizeof(char) * 4) == 0)
                {
                    cpu.setC(true);
                }
                else
                {
                    cpu.setC(false);
                }
            }
            break;
            case 'B':
            {
                if (cpu.getC())
                {
                    cpu.setIC(cpu.getOperand());
                }
            }
            break;
            case 'G':
            {
                cpu.setSI(1);
                M.setMemory(memory);
                MOS();
            }
            break;
            case 'P':
            {
                cpu.setSI(2);
                M.setMemory(memory);
                MOS();
            }
            break;
            default:
            {
                cout << "Invalid opcode" << endl;
                exit(0);
            }
            }

            M.setMemory(memory);
        }
    }

    void MOS()
    {
        switch (cpu.getSI())
        {
        case 1:
        {
            READ();
        }
        break;
        case 2:
        {
            WRITE();
        }
        break;
        case 3:
        {
            cpu.setSI(0);
            TERMINATE();
        }
        break;
        default:
        {
            cout << "Invalid SI" << endl;
            exit(0);
        }
        }
    }

    void TERMINATE()
    {
        output << "\n\n";
    }

    void WRITE()
    {
        // take the data from the memory and put it into the output file
        string line;
        char(*memory)[4] = M.getMemory();
        int oprand = cpu.getOperand();

        // converting the last bit to 0
        if (oprand % 10 != 0)
        {
            // convert that last bit to 0
            oprand = oprand - (oprand % 10);
        }

        for (int i = oprand; i < oprand + 10; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (memory[i][j] != '\0')
                {
                    line += memory[i][j];
                }
            }
        }

        output << line << endl;
        cpu.setSI(0);
    }

    void READ()
    {
        string line = "";

        getline(input, line);

        if (line.empty())
        {
            cout << "Input file is empty" << endl;
            exit(0);
        }
        else if (line == "$END")
        {
            cout << "Out of Data Card" << endl;
        }

        char *buffer = const_cast<char *>(line.c_str());
        char(*memory)[4] = M.getMemory();
        int oprand = cpu.getOperand();

        // converting the last bit to 0
        if (oprand % 10 != 0)
        {
            // convert that last bit to 0
            oprand = oprand - (oprand % 10);
        }

        // putting the whole buffer starting from the given operand address
        for (int i = 0; i < line.length();)
        {
            memory[oprand][i % 4] = buffer[i];
            i++;
            if (i % 4 == 0)
            {
                oprand++;
            }
        }

        M.setMemory(memory);
        cpu.setSI(0);
    }

    void printMemory()
    {
        char(*m)[4] = M.getMemory();
        cout << "Printing the memory..." << endl;
        cout << endl;

        // print memory in form of boxes
        for (int i = 0; i < 100; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (m[i][j] == 0)
                {
                    cout << " - ";
                }
                else
                {
                    cout << " " << m[i][j] << " ";
                }
                if (j == 3)
                {
                    cout << endl;
                }
                else
                {
                    cout << "|";
                }
            }
            cout << endl;
        }
    }
};

int main()
{
    SPARS spars("input.txt", "output.txt");
    cout << "\nOS initialized" << endl;
    spars.init();
    spars.LOAD();

    return 0;
}
