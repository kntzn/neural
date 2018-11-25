#pragma once
#include "dynarray.h"

typedef unsigned int uint;

class Neuron
    {
    private:
        darray <Neuron*> inputs;
        darray <double> weights;
        double output;
    public:
        Neuron ();
        Neuron (darray <double> weightsOfInputs);
        ~Neuron ();
    
        void  setInput (double value);
        void  calcOutput ();
        double getOutput ();

        void addNewInput (Neuron* neuron);

        void mutate (double mutationStrength, double percOfNeurons);

        void destroyInputs ();

        darray <double> getWeights ();
    };