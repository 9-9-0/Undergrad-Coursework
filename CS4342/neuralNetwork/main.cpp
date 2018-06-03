#include "neuron.h"
#include "csvReader.h"
#include <string>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <algorithm>
using namespace std;

void print_help();
string to_string_vector(vector<double> vec);
int place_of_max(vector<double> vec);
string to_string_vector(vector<string> vec);

int main(int argc, char* argv[]) {
  int inputCount=1,outputCount=1,hiddenLayerCount=1,hiddenLayerNeuronCount=1,sampleCount=1,mode=0;
  double learningRate = 0.5, errorLimit = 0.05;
  string fileName,testFileName = "",initialWeightFile="";
  
  char c;
  while ((c = getopt(argc, argv, "ho:l:n:r:f:e:t:m:w:")) != -1) {
    switch(c) {
      case 'h':
        print_help();
        return 0;
        break;
      case 'i':
        inputCount = stoi(optarg,nullptr,10);
        break;
      case 'o':
        outputCount = stoi(optarg,nullptr,10);
        break;
      case 'l':
        hiddenLayerCount = stoi(optarg,nullptr,10);
        break;
      case 'n':
        hiddenLayerNeuronCount = stoi(optarg,nullptr,10);
        break;
      case 'r':
        learningRate = strtof(optarg,nullptr);
        break;
      case 'e':
        errorLimit = strtof(optarg,nullptr);
        break;
      case 'f':
        fileName = optarg;
        break;
      case 't':
        testFileName = optarg;
        break;
      case 'w':
        initialWeightFile = optarg;
        break;
      case 'm':
        mode = stoi(optarg,nullptr,10);
        if (mode > 2) { 
          cerr << "Invalid mode: 0-2" << endl;
          exit(-1);
        }
        break;
      case ':':
        cout << optarg << " requires an argument" << endl;
        break;
    }
  }
  
  /* Read in initial weights file if exists */
  vector<double> init_weights;
  if (initialWeightFile != "") {
    vector<vector<string>> weights_file;
    ifstream weightfile;
    weightfile.open(initialWeightFile, ifstream::in);
    if (!weightfile) {
      cerr << "Error opening weights file; continuing without setting" << endl;
    } else {
      weights_file = readCSV(weightfile);
      if (weights_file.size() > 0) {
        for (int i=0; i<weights_file[0].size(); i++) {
          init_weights.push_back(strtof(weights_file[0][i].c_str(), nullptr));
        }
      } else {
        cout << "no lines in weights file" << endl;
      }
    }
    weightfile.close();
  }
  
  /* Read in training or testing data */
  vector<vector<string>> csv_file;
  vector<vector<double>> inputs;
  vector<vector<double>> expected;
  ifstream infile;
  sampleCount = 2;
  infile.open(fileName, ifstream::in);
  csv_file = readCSV(infile);
  int outCount = 0;
  for (int i=0; i<csv_file.size() && i < sampleCount+1; i++) {
    for (int j=0; j<csv_file[i].size();j++) {
      if (i==0) {
        if (j > 0) {
            break;
        }
        inputCount = stoi(csv_file[i][0], nullptr, 10);
        sampleCount = stoi(csv_file[i][1], nullptr, 10);
        outputCount = stoi(csv_file[i][2], nullptr, 10);
        inputs.resize(sampleCount, vector<double>(inputCount,0));
        expected.resize(sampleCount, vector<double>(outputCount,0));
      } else if (j >= csv_file[i].size() - outputCount) {
        expected[i-1][csv_file[i].size() - j - 1] = strtof(csv_file[i][j].c_str(), nullptr);
      } else {
        if (i - 1 < sampleCount) {
          string tmp = csv_file[i][j];
          inputs[i-1][j] = strtof(tmp.c_str(), nullptr);
        }
      }
    }
  }
  infile.close();
  
  // Neural net: input, output, hidden layers, neurons per hidden layer, learning rate
  NeuralNet* nn = new NeuralNet(inputCount, outputCount, hiddenLayerCount,hiddenLayerNeuronCount,learningRate);
  if (init_weights.size() > 0) {
    nn->setWeights(init_weights);
  }
  
  /* Training mode */
  if (mode == 0) {
    nn->trainAll(inputs, expected);
    // initialize error and epoch
    double error = 100;
    int epoch = 0;
    while (error > errorLimit) {
      cout << "Epoch " << epoch << endl;
      error = 0;
      error = nn->trainAll(inputs, expected);
      epoch++;
      //if (epoch % 1000 == 0)
      cout << "error = " << error << endl;
    }
    cout << "Error = " << error << " after " << epoch << " epochs" << endl;
    cout << "If you want your neural network to be this accurate, use the weights " << endl << to_string_vector(nn->getWeights()) << endl;
    
    if (initialWeightFile != "") {
      ofstream weightfile;
      weightfile.open(initialWeightFile, ofstream::out);
      if (!weightfile) {
        cerr << "Error opening weights file; continuing without writing" << endl;
      } else {
        weightfile << to_string_vector(nn->getWeights()) << endl;
      }
      weightfile.close();
    }
  }
  
  /*  Testing mode */
  if (mode == 1) {
    int corCount = 0,corA = 0,corNot=0,incorA=0,incorNot=0;
    for (int i=0; i<inputs.size(); i++) {
      cout << "TEST " << i << ":" << endl;
      vector<double> outputs;
      outputs = nn->update(inputs[i]);
      //cout << "  Inputs: " << to_string_vector(inputs[i]) << endl;
      cout << "  Outputs: " << to_string_vector(outputs) << endl;
      cout << "  Expects: " << to_string_vector(expected[i]) << endl;
      if (round(outputs[0]) == expected[i][0]) {
        if (round(expected[i][0]) == 1) {
          corA++;
        } else {
          corNot++;
        }
        cout << "Correct" << endl;
        corCount++;
      } else {
        if (round(expected[i][0]) == 1) {
          incorA++;
        } else {
          incorNot++;
        }
        cout << "Incorrect" << endl;
      }
       cout << endl;
    }
    cout << corCount << " out of " << inputs.size() << " correct" << endl;
    cout << corA << " correct A predictions, " << incorA << " incorrect A predictions" << endl;
    cout << corNot << " correct not A predictions, " << incorNot << " incorrect not predictions" << endl;
    cout << ((double)corCount / (double)inputs.size()) * 100.0 << "\% correct" << endl;
  }
  
  /*  Using mode */
  if (mode == 2) {
    for (int i=0; i<inputs.size(); i++) {
      vector<double> outputs;
      outputs = nn->update(inputs[i]);
      cout << to_string_vector(outputs) << endl;
    }
  }
  return 0;
}

string to_string_vector(vector<double> vec) {
  string str = "";
  for (int i=0; i<vec.size()-1; i++) {
    str += to_string(vec[i]) + ", ";
  }
  str += to_string(vec[vec.size() -1]);
  return str;
}

string to_string_vector(vector<string> vec) {
  string str = "";
  for (int i=0; i<vec.size()-1; i++) {
    str += vec[i] + ", ";
  }
  str += vec[vec.size() -1];
  return str;
}

int place_of_max(vector<double> vec) {
  int place = 0;
  double max = vec[0];
  for (int i=0; i<vec.size(); i++) {
    if (vec[i] > max) {
      max = vec[i];
      place = i;
    }
  }
  return place;
}

void print_help() {
  cout << "Usage: ./neural-network <arguments>" << endl;
  cout << endl;
  cout << "    -h: print this help and exit" << endl;
  cout << "    -i: specify number of inputs" << endl;
  cout << "    -o: specify number of outputs" << endl;
  cout << "    -l: specify number of hidden layers" << endl;
  cout << "    -n: specify number of neurons in hidden layers" << endl;
  cout << "    -r: specify learning rate" << endl;
  cout << "    -e: specify error limit at which the neural network should be considered trained" << endl;
  cout << "    -f: specify datafile name" << endl;
  cout << "    -w: specify initial weights file name, or a file to write the weights to after training" << endl;
  cout << "    -m: specify mode" << endl;
  cout << "         0: train" << endl;
  cout << "         1: test" << endl;
  cout << "         2: production" << endl;
  cout << endl;
  exit(0);
}
