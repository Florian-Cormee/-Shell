#include "utils.h"

size_t length(char **tab)
{
    size_t size = 0;
    while (tab[size] != NULL)
    {
        size++;
    }
    return size;
}
