#ifndef NEURON_H
#define NEURON_H
#include <vector>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <iostream>
using namespace std;

class Neuron {
  public:
    vector<double> lastInputs;
    double lastOutput;
    double delta;
    int inputCount;
    vector<double> weights;
    Neuron(int);
    int getInputCount();
    void setInputCount(int inputCount);
    vector<double> getWeights();
    void setWeights(vector<double>);
    void updateWeightsFromDelta(double learningRate);
    void setDelta(double delta);
    double getOutput(vector<double> inputs);
};

class NeuronLayer {
  public:
    int neuronCount;
    vector<Neuron> neurons;
    NeuronLayer(int, int);
    int getNeuronCount();
    void setNeuronCount(int neuronCount);
    vector<Neuron> getNeurons();
    void setNeurons(vector<Neuron>);
};

class NeuralNet {
  double learningRate;
  int inputCount;
  int outputCount;
  int hiddenLayerCount;
  int hiddenLayerNeuronCount;
  vector<NeuronLayer> layers;
  public:
    NeuralNet(int inputCount, int outputCount, int hiddenLayerCount, int hiddenLayerNeuronCount, double learningRate);
    vector<double> getWeights();
    int getWeightCount();
    void setWeights(vector<double> &weights);
    vector<double> update(vector<double> inputs);
    double sigmoid(double activation, double response);
    void backpropagate(vector<double> expected);
    double train(vector<double> &inputs, vector<double> &expectedValues);
    double trainAll(vector<vector<double>> &inputs, vector<vector<double>> &expectedValues);
};

#endif
