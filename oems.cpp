#include <iostream>
#include <vector>
#include <fstream>
#include <tuple>
#include <mpi.h>

#define TAG 69

using namespace std;

// Print numbers in required format (with space or new line between them)
bool customPrint(bool withNewLine, vector<int> nums) {
        for(int i=0; i < nums.size(); i++) {
                cout << nums.at(i);
                if (withNewLine) {
                        cout << endl;
                } else if (i+1 != nums.size()){
                        cout << " ";
                }
        }
}

bool loadAndPrintNumbers(vector<int> *nums) {
        // Open input file, load numbers
        ifstream file;
        file.open("numbers");
        if (!file) {
                cerr << "Error while opening file 'numbers'" << endl;
                exit(1);
        }

        if (file.is_open()) {
                int num;
                while (file) {
                        num = (int) file.get();
                        if (file.eof())	break;
                        nums->push_back(num);
                }
        } else {
                cerr << "Error while opening file 'numbers'" << endl;
                exit(1);
        }
        file.close();

        // Print numbers
        customPrint(false, *nums);
        cout << endl;

        return 0;
}

// CE 1x1
// Merges two sorted halves of a sequence to a completely sorted sequence (length 2)
int* sort1x1(int a, int b) {
        static int result[2];
        if (a < b) {
                result[0] = a;
                result[1] = b;
        } else {
                result[0] = b;
                result[1] = a;
        }
        return result;
}

// MPI send one integer to specific process (dest)
bool mpiSend(int num, int dest) {
        return MPI_Send(&num, 1, MPI_INT, dest, TAG, MPI_COMM_WORLD);
}

// MPI recieve two integers from sender
bool mpiRecv2(int *buf, int sender) {
        MPI_Status status;
        return MPI_Recv(buf, 2, MPI_INT, sender, TAG, MPI_COMM_WORLD, &status);
}

// MPI recieve integer from sender
bool mpiRecv(int *buf, int sender) {
        MPI_Status status;
        return MPI_Recv(buf, 1, MPI_INT, sender, TAG, MPI_COMM_WORLD, &status);
}

// Get sorted numbers, each from specific process (sender)
bool getSortedNumbers(int sender, vector<int> *sortedNums) {
        int recvNum;
        MPI_Status status;
        MPI_Recv(&recvNum, 1, MPI_INT, sender, TAG, MPI_COMM_WORLD, &status);
        sortedNums->push_back(recvNum);
        return 0;
}

int main(int argc, char** argv) {
        int rank, size, recvNum1, recvNum2;
        int recvNums[2];
        vector<int> nums, sortedNums;

        // MPI init, get ranks
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Status status;

        // Distribution of work between processes
        switch (rank) {
                case 0: {
                        // Process 0 loads numbers from input file
                        loadAndPrintNumbers(&nums);

                        // Send numbers to other processes
                        for (int i = 1; i < 4; i++) {
                                int values[2];
                                values[0] = nums[2*i];
                                values[1] = nums[2*i+1];
                                MPI_Send(values, 2, MPI_INT, i, TAG, MPI_COMM_WORLD);
                        }

                        // Sort first two of them
                        int *sorted = sort1x1(nums[0], nums[1]);
                        mpiSend(sorted[0], 4);
                        mpiSend(sorted[1], 5);

                        getSortedNumbers(10, &sortedNums);
                        getSortedNumbers(16, &sortedNums);
                        getSortedNumbers(16, &sortedNums);
                        getSortedNumbers(17, &sortedNums);
                        getSortedNumbers(17, &sortedNums);
                        getSortedNumbers(18, &sortedNums);
                        getSortedNumbers(18, &sortedNums);
                        getSortedNumbers(13, &sortedNums);

                        // Print sorted numbers
                        customPrint(true, sortedNums);
                        break;
                } case 1: {
                        mpiRecv2(recvNums, 0);
                        int *sorted = sort1x1(recvNums[0], recvNums[1]);
                        mpiSend(sorted[0], 4);
                        mpiSend(sorted[1], 5);
                        break;
                } case 2: {
                        mpiRecv2(recvNums, 0);
                        int *sorted = sort1x1(recvNums[0], recvNums[1]);
                        mpiSend(sorted[0], 6);
                        mpiSend(sorted[1], 7);
                        break;
                } case 3: {
                        mpiRecv2(recvNums, 0);
                        int *sorted = sort1x1(recvNums[0], recvNums[1]);
                        mpiSend(sorted[0], 6);
                        mpiSend(sorted[1], 7);
                        break;

// ######################## 2nd BLOCK ########################
                } case 4: {
                        mpiRecv(&recvNum1, 0);
                        mpiRecv(&recvNum2, 1);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 10);
                        mpiSend(sorted[1], 8);
                        break;
                } case 5: {
                        mpiRecv(&recvNum1, 0);
                        mpiRecv(&recvNum2, 1);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 8);
                        mpiSend(sorted[1], 13);
                        break;
                } case 6: {
                        mpiRecv(&recvNum1, 2);
                        mpiRecv(&recvNum2, 3);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 10);
                        mpiSend(sorted[1], 9);
                        break;
                } case 7: {
                        mpiRecv(&recvNum1, 2);
                        mpiRecv(&recvNum2, 3);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 9);
                        mpiSend(sorted[1], 13);
                        break;
                } case 8: {
                        mpiRecv(&recvNum1, 4);
                        mpiRecv(&recvNum2, 5);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 12);
                        mpiSend(sorted[1], 11);
                        break;
                } case 9: {
                        mpiRecv(&recvNum1, 6);
                        mpiRecv(&recvNum2, 7);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 12);
                        mpiSend(sorted[1], 11);
                        break;

// ######################## 3rd BLOCK ########################
                } case 10: {
                        mpiRecv(&recvNum1, 4);
                        mpiRecv(&recvNum2, 6);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 0);
                        mpiSend(sorted[1], 14);
                        break;
                } case 11: {
                        mpiRecv(&recvNum1, 8);
                        mpiRecv(&recvNum2, 9);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 14);
                        mpiSend(sorted[1], 18);
                        break;
                } case 12: {
                        mpiRecv(&recvNum1, 8);
                        mpiRecv(&recvNum2, 9);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 16);
                        mpiSend(sorted[1], 15);
                        break;
                } case 13: {
                        mpiRecv(&recvNum1, 5);
                        mpiRecv(&recvNum2, 7);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 15);
                        mpiSend(sorted[1], 0);
                        break;
                } case 14: {
                        mpiRecv(&recvNum1, 10);
                        mpiRecv(&recvNum2, 11);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 16);
                        mpiSend(sorted[1], 17);
                        break;
                } case 15: {
                        mpiRecv(&recvNum1, 12);
                        mpiRecv(&recvNum2, 13);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 17);
                        mpiSend(sorted[1], 18);
                        break;
                } case 16: {
                        mpiRecv(&recvNum1, 14);
                        mpiRecv(&recvNum2, 12);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 0);
                        mpiSend(sorted[1], 0);
                        break;
                } case 17: {
                        mpiRecv(&recvNum1, 14);
                        mpiRecv(&recvNum2, 15);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 0);
                        mpiSend(sorted[1], 0);
                        break;
                } case 18: {
                        mpiRecv(&recvNum1, 11);
                        mpiRecv(&recvNum2, 15);
                        int *sorted = sort1x1(recvNum1, recvNum2);
                        mpiSend(sorted[0], 0);
                        mpiSend(sorted[1], 0);
                        break;
                }
        }

        MPI_Finalize();
        return 0;
}