#pragma once
#include "dynarray.h"
#include "Neuron.h"



class Network
    {
    private:
        uint N_inp = 0;
        uint N_hid = 0;
        uint N_out = 0;

        darray <Neuron*> neurons;

    public:
        Network (uint nInputs, uint nHidden, uint nOutputs);
        Network (Network netA, Network netB);
        Network (char filename []);
        ~Network ();
    
        void update ();

        double getOuput (uint id);
        darray <Neuron*> getNeurons ();
        void setInput (uint id, double value);

        void mutate (double strength, double rate);

        uint getInputLayerSize ();
        uint getHiddenLayerSize ();
        uint getOutputLayerSize ();

        void saveToFile (char filename []);

        double version ()
            { 
            return 1.1;
            }
    };

