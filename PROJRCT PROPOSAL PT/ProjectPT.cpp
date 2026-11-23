/*
============================================================================
Filename    : DiskSchedulingSimulator.cpp
Project     : PT 101 Final Project - Topic 4: Disk Scheduling
University  : Batangas State University, Alangilan Campus
Course      : Platform Technologies (PT 101)
Description : Simulates FCFS, SSTF, and SCAN disk scheduling algorithms.
              Calculates total head movement and shows detailed results.
============================================================================
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>

using namespace std;

// ====================== DATA STRUCTURES ======================
struct DiskData {
    int initialHead;
    vector<int> requests;
    int minCylinder;
    int maxCylinder;
    string initialDirection;
};

struct AlgorithmResult {
    string name;
    vector<int> sequence;
    vector<int> movement;
    int totalMovement;
};

// ====================== HELPER FUNCTIONS ======================
int validateInteger(string prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value >= minVal && value <= maxVal) {
                return value;
            } else {
                cout << "Please enter a value between " << minVal << " and " << maxVal << ".\n";
            }
        } else {
            cout << "Invalid input! Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

string validateDirection() {
    string dir;
    while (true) {
        cout << "Enter initial head direction (LEFT/RIGHT): ";
        cin >> dir;
        for (char &c : dir) c = toupper(c);
        if (dir == "LEFT" || dir == "RIGHT") return dir;
        cout << "Invalid direction! Please enter LEFT or RIGHT.\n";
    }
}

void printHeader(string title) {
    cout << "\n==================================================\n";
    cout << "                  " << title << "\n";
    cout << "==================================================\n";
}

void printResult(const AlgorithmResult &res) {
    cout << "\nAlgorithm: " << res.name << "\n";
    
    cout << "Service Sequence : ";
    for (size_t i = 0; i < res.sequence.size(); i++) {
        cout << res.sequence[i];
        if (i != res.sequence.size() - 1) cout << " -> ";
    }
    cout << "\n";
    
    cout << "Head Movement   : ";
    for (size_t i = 0; i < res.movement.size(); i++) {
        cout << res.movement[i];
        if (i != res.movement.size() - 1) cout << " + ";
    }
    cout << "\n";
    
    cout << "Total Movement  : " << res.totalMovement << " cylinders\n";
}

// ====================== ALGORITHM 1: FCFS ======================
AlgorithmResult calculateFCFS(const DiskData &data) {
    AlgorithmResult result;
    result.name = "FCFS (First-Come, First-Served)";
    int currentPosition = data.initialHead;
    
    result.sequence.push_back(currentPosition);
    
    for (int req : data.requests) {
        result.sequence.push_back(req);
        int distance = abs(currentPosition - req);
        result.movement.push_back(distance);
        result.totalMovement += distance;
        currentPosition = req;
    }
    return result;
}

// ====================== ALGORITHM 2: SSTF ======================
AlgorithmResult calculateSSTF(const DiskData &data) {
    AlgorithmResult result;
    result.name = "SSTF (Shortest Seek Time First)";
    vector<bool> served(data.requests.size(), false);
    int currentPosition = data.initialHead;
    int countServed = 0;
    
    result.sequence.push_back(currentPosition);
    
    while (countServed < data.requests.size()) {
        int minDistance = 99999;
        int selectedIndex = -1;
        
        for (int i = 0; i < data.requests.size(); i++) {
            if (!served[i]) {
                int distance = abs(currentPosition - data.requests[i]);
                if (distance < minDistance) {
                    minDistance = distance;
                    selectedIndex = i;
                }
            }
        }
        
        if (selectedIndex != -1) {
            served[selectedIndex] = true;
            countServed++;
            result.sequence.push_back(data.requests[selectedIndex]);
            result.movement.push_back(minDistance);
            result.totalMovement += minDistance;
            currentPosition = data.requests[selectedIndex];
        }
    }
    return result;
}

// ====================== ALGORITHM 3: SCAN ======================
AlgorithmResult calculateSCAN(const DiskData &data) {
    AlgorithmResult result;
    result.name = "SCAN (Elevator) - Direction: " + data.initialDirection;
    vector<int> leftSide, rightSide;
    int currentPosition = data.initialHead;
    
    for (int req : data.requests) {
        if (req < currentPosition) leftSide.push_back(req);
        else if (req > currentPosition) rightSide.push_back(req);
    }
    
    sort(leftSide.rbegin(), leftSide.rend());
    sort(rightSide.begin(), rightSide.end());
    
    result.sequence.push_back(currentPosition);
    
    if (data.initialDirection == "RIGHT") {
        for (int r : rightSide) {
            result.sequence.push_back(r);
            result.movement.push_back(abs(currentPosition - r));
            result.totalMovement += abs(currentPosition - r);
            currentPosition = r;
        }
        if (currentPosition != data.maxCylinder) {
            result.sequence.push_back(data.maxCylinder);
            result.movement.push_back(abs(currentPosition - data.maxCylinder));
            result.totalMovement += abs(currentPosition - data.maxCylinder);
            currentPosition = data.maxCylinder;
        }
        for (int l : leftSide) {
            result.sequence.push_back(l);
            result.movement.push_back(abs(currentPosition - l));
            result.totalMovement += abs(currentPosition - l);
            currentPosition = l;
        }
    } else {
        for (int l : leftSide) {
            result.sequence.push_back(l);
            result.movement.push_back(abs(currentPosition - l));
            result.totalMovement += abs(currentPosition - l);
            currentPosition = l;
        }
        if (currentPosition != data.minCylinder) {
            result.sequence.push_back(data.minCylinder);
            result.movement.push_back(abs(currentPosition - data.minCylinder));
            result.totalMovement += abs(currentPosition - data.minCylinder);
            currentPosition = data.minCylinder;
        }
        for (int r : rightSide) {
            result.sequence.push_back(r);
            result.movement.push_back(abs(currentPosition - r));
            result.totalMovement += abs(currentPosition - r);
            currentPosition = r;
        }
    }
    return result;
}

// ====================== COMPARISON SUMMARY ======================
void printComparison(const AlgorithmResult &fcfs, const AlgorithmResult &sstf, const AlgorithmResult &scan) {
    printHeader("COMPARISON SUMMARY");
    cout << left << setw(35) << "ALGORITHM" << setw(20) << "TOTAL HEAD MOVEMENT\n";
    cout << "-----------------------------------------------------------------\n";
    cout << left << setw(35) << fcfs.name << setw(20) << fcfs.totalMovement << "\n";
    cout << left << setw(35) << sstf.name << setw(20) << sstf.totalMovement << "\n";
    cout << left << setw(35) << scan.name << setw(20) << scan.totalMovement << "\n";
    cout << "-----------------------------------------------------------------\n";
    
    int minMove = min(min(fcfs.totalMovement, sstf.totalMovement), scan.totalMovement);
    cout << "Most Efficient: ";
    if (minMove == fcfs.totalMovement) cout << "FCFS";
    else if (minMove == sstf.totalMovement) cout << "SSTF";
    else cout << "SCAN";
    cout << " (Least Head Movement)\n";
}

// ====================== MAIN PROGRAM ======================
int main() {
    cout << "=================================================================\n";
    cout << "         DISK SCHEDULING SIMULATOR - FCFS | SSTF | SCAN\n";
    cout << "              PT 101 - Platform Technologies | Topic 4\n";
    cout << "=================================================================\n\n";
    
    DiskData input;
    
    cout << "--- DISK INFORMATION ---\n";
    input.minCylinder = validateInteger("Enter minimum cylinder number: ", 0, 1000000);
    input.maxCylinder = validateInteger("Enter maximum cylinder number: ", input.minCylinder + 1, 1000000);
    
    input.initialHead = validateInteger("Enter initial head position: ", input.minCylinder, input.maxCylinder);
    
    int numRequests = validateInteger("Enter number of cylinder requests: ", 1, 1000);
    cout << "Enter " << numRequests << " cylinder request(s):\n";
    for (int i = 0; i < numRequests; i++) {
        int cyl = validateInteger("  Request " + to_string(i+1) + ": ", input.minCylinder, input.maxCylinder);
        input.requests.push_back(cyl);
    }
    
    input.initialDirection = validateDirection();
    
    printHeader("INPUT SUMMARY");
    cout << "Disk Range       : " << input.minCylinder << " to " << input.maxCylinder << "\n";
    cout << "Initial Head     : " << input.initialHead << "\n";
    cout << "Request Queue    : ";
    for (size_t i = 0; i < input.requests.size(); i++) {
        cout << input.requests[i];
        if (i != input.requests.size() - 1) cout << ", ";
    }
    cout << "\nInitial Direction: " << input.initialDirection << "\n";
    
    printHeader("SIMULATION RESULTS");
    AlgorithmResult fcfsResult = calculateFCFS(input);
    printResult(fcfsResult);
    
    AlgorithmResult sstfResult = calculateSSTF(input);
    printResult(sstfResult);
    
    AlgorithmResult scanResult = calculateSCAN(input);
    printResult(scanResult);
    
    printComparison(fcfsResult, sstfResult, scanResult);
    
    cout << "\nSimulation Completed Successfully!\n";
    return 0;
}
