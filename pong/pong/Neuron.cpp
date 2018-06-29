#include "Neuron.h"

#define e_num 2.71828182846

double weightRand ()
    {
    return (rand ()%2001 - 1000.0)/1000.0;
    }


Neuron::Neuron ()
    { }
Neuron::Neuron (std::vector<double> weightsOfInputs)
    { 
    weights = weightsOfInputs;
    }
Neuron::~Neuron ()
    { }

void Neuron::setInput (double value)
    {
    output = value;
    }

void Neuron::calcOutput ()
    { 
    output = 0;

    for (int i = 0; i < inputs.size (); i++)
        {
        output += weights [i]*inputs [i]->getOutput ();
        }

    output = 1/(1+pow (e_num, -output));
    }

double Neuron::getOutput ()
    { 
    return output;
    }

void Neuron::addNewInput (Neuron* neuron)
    {
    inputs.push_back (neuron);
    weights.push_back (weightRand ());
    }

void Neuron::mutate (double mutationStrength, double percOfNeurons)
    { 
    for (int i = 0; i < weights.size(); i++)
        if (rand ()%1000000 < int (percOfNeurons*1000000))
            weights [i] += weightRand ()*mutationStrength;

    }

void Neuron::destroyInputs ()
    { 
    inputs.clear ();
    }

std::vector<double> Neuron::getWeights ()
    {
    return weights;
    }
