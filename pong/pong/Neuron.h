#pragma once
#include <vector>

typedef unsigned int uint;

class Neuron
    {
    private:
        std::vector <Neuron*> inputs;
        std::vector <double> weights;
        double output;
    public:
        Neuron ();
        ~Neuron ();
    
        void  setInput (double value);
        void  calcOutput ();
        double getOutput ();

        void addNewInput (Neuron* neuron);

        void mutate (double mutationStrength, double percOfNeurons);

        void destroyInputs ();
    };