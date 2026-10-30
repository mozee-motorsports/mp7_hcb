#include "sevcon_pdo.h"
#include <cassert>

int main()
{
    SevconPdoLayouts maps;
    assert(SevconPdo_DefaultLayouts(&maps) == PDO_OK);
    assert(SevconPdo_ValidateLayouts(&maps) == PDO_OK);
    SevconPdoFeedback feedback;
    SevconPdo_ClearFeedback(&feedback);
    assert(!SevconPdo_IsFresh(&feedback, SEVCON_TPDO1, 0, 20));
    return 0;
}
