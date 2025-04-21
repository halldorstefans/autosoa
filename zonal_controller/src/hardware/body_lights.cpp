#include "body_lights.h"

namespace Body
{
    Lights::Lights() : is_headlight_on_(false)
    {
    }

    bool Lights::set_headlight(bool state)
    {
        is_headlight_on_ = state;

        return true;
    }

    bool Lights::get_headlight_state()
    {
        return is_headlight_on_;
    }
}
