#include "Network.h"

int main ()
    {
    Network net (2, 4, 1);

    net.setInput (0, 1);
    net.setInput (1, 0);

    net.update ();

    printf ("%f\n", net.getOuput (0));
    system ("pause");
    }