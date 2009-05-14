/// \file hud.hpp
/// \brief Heads up display.
/// \author Ben Radford 
/// \date 14th May 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef HUD_HPP
#define HUD_HPP


#include <CEGUI.h>


namespace gfx {


class HUD {
    public:
        HUD(CEGUI::System& cegui);

    private:
        CEGUI::System& _cegui;

};


}  // namespace gfx


#endif  // HUD_HPP

