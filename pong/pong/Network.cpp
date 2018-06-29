#include "Network.h"

Network::Network (uint nInputs, uint nHidden, uint nOutputs)
    { 
    N_inp = nInputs;
    N_hid = nHidden;
    N_out = nOutputs;

    for (int i = 0; i < N_inp; i++)
        {
        neurons.push_back (new Neuron);
        }
    for (int i = N_inp; i < N_inp+N_hid; i++)
        {
        neurons.push_back (new Neuron);
        for (int j = 0; j < N_inp; j++)
            neurons [i]->addNewInput (neurons [j]);
        }
    for (int i = N_inp+N_hid; i < N_inp+N_hid+N_out; i++)
        {
        neurons.push_back (new Neuron);
        for (int j = N_inp; j < N_hid+N_inp; j++)
            neurons [i]->addNewInput (neurons [j]);
        }

    }
Network::Network (Network netA, Network netB)
    { 
    uint sizeA = netA.getNeurons ().size ();
    uint sizeB = netB.getNeurons ().size ();

    if (sizeA == sizeB)
        {
        N_inp = netA.getInputLayerSize ();
        N_hid = netA.getHiddenLayerSize ();
        N_out = netA.getOutputLayerSize ();

        // Cross-over between two networks
        uint swapPoint = rand()%sizeA;

        for (int i = 0; i < sizeA; i++)
            {
            if (i < swapPoint)
                neurons.push_back (new Neuron (*netB.getNeurons () [i]));
            else
                neurons.push_back (new Neuron (*netA.getNeurons () [i]));
            
            neurons.back ()->destroyInputs ();
            }
       
        for (int i = N_inp; i < N_inp+N_hid; i++)
            for (int j = 0; j < N_inp; j++)
                neurons [i]->addNewInput (neurons [j]);
            
        for (int i = N_inp+N_hid; i < N_inp+N_hid+N_out; i++)
            for (int j = N_inp; j < N_hid+N_inp; j++)
                neurons [i]->addNewInput (neurons [j]);
            
        }
    else
        {
        printf ("Networks have different size\n");
        exit (1);
        this->~Network ();
        }
    }
Network::~Network ()
    { }

void Network::update ()
    { 
    for (int i = N_inp; i < neurons.size (); i++)
        neurons [i]->calcOutput ();
    }

double Network::getOuput (uint id)
    {
    if (id < N_out)
        return neurons [id+N_inp+N_hid]->getOutput ();
    else
        printf ("in 'void setgetOutput': '%d' is incorrect id\n", id);

    return NAN;
    }
std::vector<Neuron*> Network::getNeurons ()
    {
    return neurons;
    }
void Network::setInput (uint id, double value)
    {
    if (id < N_inp)
        neurons [id]->setInput (value);
    else
        printf ("in 'void setInput': '%d' is incorrect id\n", id);
    }

// strength - maximum delta of weight
// rate - value that sets percentage of mutated neurons [0; 1]
void Network::mutate (double strength, double rate)
    { 
    for (int i = 0; i < neurons.size (); i++)
        neurons [i]->mutate (strength, rate);
    }

uint Network::getInputLayerSize ()
    {
    return N_inp;
    }
uint Network::getHiddenLayerSize ()
    {
    return N_hid;
    }
uint Network::getOutputLayerSize ()
    {
    return N_out;
    }