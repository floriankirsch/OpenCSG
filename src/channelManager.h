// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2004
// Hasso-Plattner-Institute at the University of Potsdam, Germany, and Florian Kirsch
//
// This library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License, 
// Version 2, as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

//
// channelManager.h 
//
// manage resources for temporary CSG results
//

#ifndef __OpenCSG__channel_manager_h__
#define __OpenCSG__channel_manager_h__

#include <opencsgConfig.h>
#include <utility>
#include <vector>

class RenderTexture;

namespace OpenCSG {

    class Primitive;

    enum Channel {
        NoChannel = 0, Alpha = 1, Red = 2, Green = 4, Blue = 8
    };

    class ChannelManager {
    public:
        ChannelManager();
            // A pbuffer is used to collect CSG results in its four color
            // channels. These resources are managed in a ChannelManager
            // object. Since we use one pbuffer only, this class is a singleton
        virtual ~ChannelManager();

        Channel find() const;
            // returns a free channel, or NoChannel if nothings available
        Channel request();
            // allocates a new channel for temporary calculation of visibility
            // information. return NoChannel if no free channel was found.
        Channel current() const;
            // returns the currently used channel for visibility calculation
        std::vector<Channel> occupied() const;
            // returns channels that currently contain visibility information
        void free();
            // releases the pbuffer and invokes merge() to transfer the
            // visibility information into the main canvas. 
        virtual void merge() = 0;
            // transfers visibility information into the main canvas. 
            // implemented by subclasses SCSChannelManager and 
            // GoldfeatherChannelManager

        void renderToChannel(bool on);
            // activates or deactivates rendering into a channel
        void setupProjectiveTexture();
            // setups texture stuff that makes the size of the pbuffer and the
            // size of the main canvas correspond. activates pbuffer-texture
        void resetProjectiveTexture();
            // undoes texture settings
        static void setupTexEnv(Channel channel);
            // activate texenv settings such that information in channel is
            // moved into alpha, to allow alpha testing of the channel

    private:
        static RenderTexture* pbuffer_;
        static bool inUse_;

        bool inPBuf_;
        Channel currentChannel_;
        int occupiedChannels_;
    };

    class ChannelManagerForBatches : public ChannelManager {
    public:
        ChannelManagerForBatches();

        void store(Channel channel, const std::vector<Primitive*>& primitives, int layer);
            // allows to remember which primitives are stored in which channel
            // layer == -1: convex primitives
            // layer ==  0: frontmost surface
            // etc ...
        const std::vector<Primitive*> getPrimitives(Channel channel) const;
            // returns primitives for a channel
        int getLayer(Channel channel) const;
            // returns layer for a channel
        void clear();
            // clears information

    private:
        std::vector<std::pair<std::vector<Primitive*>, int> > primitives_;
    };

} // namespace OpenCSG

#endif // __OpenCSG__channel_manager_h__


