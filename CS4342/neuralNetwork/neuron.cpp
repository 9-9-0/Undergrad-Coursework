#include "neuron.h"
/*

Neuron class stuff

*/

Neuron::Neuron(int inputCount) {
  this->lastOutput = 0;
  this->inputCount = inputCount;
  this->delta = 0;
  // Randomly assign weights when created
  for (int i=0; i<inputCount + 1; i++) {
    this->weights.push_back(((rand())/(RAND_MAX+1.0)) - ((rand())/(RAND_MAX+1.0)));
    this->lastInputs.push_back(0);
  }
}

void Neuron::updateWeightsFromDelta(double learningRate) {
  for (int i=0; i<inputCount; i++) {
    weights[i] += delta * learningRate * lastInputs[i];
  }
  weights[weights.size() -1] -= learningRate * delta;
}

void Neuron::setDelta(double delta) {
  this->delta = delta;
}

int Neuron::getInputCount() {
  return this->inputCount;
}

void Neuron::setInputCount(int inputCount) {
  this->inputCount = inputCount;
}

vector<double> Neuron::getWeights() {
  return this->weights;
}

void Neuron::setWeights(vector<double> weights) {
  this->weights.clear();
  for (int i=0; i<weights.size(); i++) {
    this->weights.push_back(weights[i]);
  }
}

double Neuron::getOutput(vector<double> inputs) {
  this->lastOutput = 0.0;
  for (int i=0; i<inputs.size(); i++) {
    this->lastOutput += inputs[i] * this->weights[i];
    this->lastInputs[i] = inputs[i];
  }
  this->lastOutput = (1 / (1 + exp(-this->lastOutput)));
  return this->lastOutput;
}

/*

NeuronLayer class

*/

NeuronLayer::NeuronLayer(int neuronCount, int inputsPerNeuron) {
  this->neuronCount = neuronCount;
  for (int i=0; i<neuronCount; i++) {
    this->neurons.push_back(Neuron(inputsPerNeuron));
  }
}

int NeuronLayer::getNeuronCount() {
  return this->neuronCount;
}

void NeuronLayer::setNeuronCount(int neuronCount) {
  this->neuronCount = neuronCount;
}

vector<Neuron> NeuronLayer::getNeurons() {
  return this->neurons;
}

void NeuronLayer::setNeurons(vector<Neuron> weights) {
  this->neurons.clear();
  for (int i=0; i<weights.size(); i++) {
    this->neurons.push_back(weights[i]);
  }
}


/*

NeuralNet class

*/

NeuralNet::NeuralNet(int inputCount, int outputCount, int hiddenLayerCount, int hiddenLayerNeuronCount, double learningRate) {
  srand(1000);
  this->inputCount = inputCount;
  this->outputCount = outputCount;
  this->hiddenLayerCount = hiddenLayerCount;
  this->hiddenLayerNeuronCount = hiddenLayerNeuronCount;
  this->learningRate = learningRate;
  // add hidden layers
  for (int i=0; i<hiddenLayerCount; i++) {
    if (i == 0) {
      this->layers.push_back(NeuronLayer(hiddenLayerNeuronCount, inputCount));
    } else {
      this->layers.push_back(NeuronLayer(hiddenLayerNeuronCount, hiddenLayerNeuronCount));
    }
  }
  // add output layer
  this->layers.push_back(NeuronLayer(outputCount, hiddenLayerNeuronCount));
}

vector<double> NeuralNet::getWeights() {
  vector<double> weights;
  
  for (int i=0; i<hiddenLayerCount + 1; i++) {
    for (int j=0; j<layers[i].neuronCount; j++) {
      for (int k=0; k<layers[i].neurons[j].inputCount; k++) {
        weights.push_back(layers[i].neurons[j].weights[k]);
      }
    }
  }
  return weights;
}

void NeuralNet::setWeights(vector<double> &weights) {
  int weight = 0;
  
  for (int i=0; i<hiddenLayerCount + 1; i++) {
    for (int j=0; j< layers[i].neuronCount; j++) {
      for (int k=0; k<layers[i].neurons[j].inputCount; k++) {
        layers[i].neurons[j].weights[k] = weights[weight++];
      }
    }
  }
}

int NeuralNet::getWeightCount() {
  int weights = 0;
  
  for (int i=0; i<hiddenLayerCount + 1; i++) {
    for (int j=0; j<layers[i].neuronCount; j++) {
      weights++;
    }
  }
  return weights;
}

vector<double> NeuralNet::update(vector<double> inputs) {
  vector<double> outputs;
  // check if input count is correct
  if (inputs.size() != inputCount) {
    return outputs;
  }
  // for each layer
  for (int i=0; i<hiddenLayerCount + 1; i++) {
    if (i > 0) {
      //set inputs to outputs at every layer but the first
      inputs = outputs;
    }
    
    outputs.clear();
    
    for (int j=0; j<layers[i].neuronCount; j++) {
      outputs.push_back(layers[i].neurons[j].getOutput(inputs));
    }
  }
  return outputs;
}

double NeuralNet::sigmoid(double activation, double response) {
  return ( 1 / ( 1 + exp(-activation)));
}

void NeuralNet::backpropagate(vector<double> expected) {
  vector<double> errors;
  for (int i=layers.size() - 1; i>=0; i--) {
    if (i != layers.size() - 1) {
      for (int j=0; j<layers[i].neuronCount; j++) {
        double err = 0;
        for (int k = 0; k<layers[i+1].neuronCount; k++) {
          err += layers[i+1].neurons[k].weights[j] * layers[i+1].neurons[k].delta;
        }
        errors.push_back(err);
      }
    } else {
      for (int j=0; j<layers[i].neuronCount; j++) {
        //cout << "Expected = " << expected[j] << " while actual was " << layers[i].neurons[j].lastOutput << " which means that there is an error of " << layers[i].neurons[j].lastOutput - expected[j] << endl;
        errors.push_back(expected[j] - layers[i].neurons[j].lastOutput);
      }
    }
    for (int j=0; j<layers[i].neuronCount; j++) {
      layers[i].neurons[j].delta = errors[j] * (layers[i].neurons[j].lastOutput * (1.0 - layers[i].neurons[j].lastOutput));
    }
    errors.clear();
  }
}

double NeuralNet::train(vector<double> &inputs, vector<double> &expectedValues) {
  vector<double> outputs;
  double totalError = 0;
  double curErr = 0;
  if (expectedValues.size() != this->outputCount) {
    return totalError;
  }
  outputs = this->update(inputs);
  for (int i=0; i<this->outputCount; i++) {
    totalError += 0.5 * pow((expectedValues[i] - outputs[i]),2);
  }
  // cout << "TotalError = " << totalError << endl;
  backpropagate(expectedValues);
  // Actually apply the updates to the weights
  for (int i=0; i<layers.size(); i++) {
    for (int j=0; j<layers[i].neuronCount; j++) {
      // cout << "Updating layer " << i << " neuron " << j << endl;
      string str = "";
      for (int k=0; k<layers[i].neurons[j].weights.size(); k++) {
        str += to_string(layers[i].neurons[j].weights[k]) + ", ";
      }
      // cout << "Old weights on neuron " << j << " in layer " << i << " = " << str << endl;
      layers[i].neurons[j].updateWeightsFromDelta(learningRate);
      str = "";
      for (int k=0; k<layers[i].neurons[j].weights.size(); k++) {
        str += to_string(layers[i].neurons[j].weights[k]) + ", ";
      }
      // cout << "New weights on neuron " << j << " in layer " << i << " = " << str << endl;
    }
  }
  return totalError;
}

double NeuralNet::trainAll(vector<vector<double>> &inputs, vector<vector<double>> &expectedValues) {
  int size = inputs.size();
  double totalError = 0;
  double curErr = 0;
  for (int i=0; i<size; i++) {
    curErr = train(inputs[i], expectedValues[i]) / size;
    //cout << curErr << endl;
    totalError += curErr;
  }
  return totalError;
}
