//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2019 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#ifndef CONSOLE_HXX
#define CONSOLE_HXX

class Event;
class Switches;
class System;
class TIA;
class M6502;
class M6532;
class Cartridge;
class CompuMate;
class Debugger;
class AudioQueue;
class AudioSettings;

#include "bspf.hxx"
#include "ConsoleIO.hxx"
#include "Control.hxx"
#include "Props.hxx"
#include "TIAConstants.hxx"
#include "FrameBuffer.hxx"
#include "Serializable.hxx"
#include "EventHandlerConstants.hxx"
#include "NTSCFilter.hxx"
#include "EmulationTiming.hxx"
#include "ConsoleTiming.hxx"
#include "frame-manager/AbstractFrameManager.hxx"

/**
  Contains detailed info about a console.
*/
struct ConsoleInfo
{
  string BankSwitch;
  string CartName;
  string CartMD5;
  string Control0;
  string Control1;
  string DisplayFormat;
};

/**
  This class represents the entire game console.

  @author  Bradford W. Mott
*/
class Console : public Serializable, public ConsoleIO
{
  public:
    /**
      Create a new console for emulating the specified game using the
      given game image and operating system.

      @param osystem  The OSystem object to use
      @param cart     The cartridge to use with this console
      @param props    The properties for the cartridge
    */
    Console(OSystem& osystem, unique_ptr<Cartridge>& cart,
            const Properties& props, AudioSettings& audioSettings);

    /**
      Destructor
    */
    virtual ~Console();

  public:
    /**
      Get the controller plugged into the specified jack

      @return The specified controller
    */
    Controller& leftController() const override { return *myLeftControl;  }
    Controller& rightController() const override { return *myRightControl; }

    /**
      Get the TIA for this console

      @return The TIA
    */
    TIA& tia() const { return *myTIA; }

    /**
      Get the properties being used by the game

      @return The properties being used by the game
    */
    const Properties& properties() const { return myProperties; }

    /**
      Get the console switches

      @return The console switches
    */
    Switches& switches() const override { return *mySwitches; }

    /**
      Get the 6502 based system used by the console to emulate the game

      @return The 6502 based system
    */
    System& system() const { return *mySystem; }

    /**
      Get the cartridge used by the console which contains the ROM code

      @return The cartridge for this console
    */
    Cartridge& cartridge() const { return *myCart; }

    /**
      Get the 6532 used by the console

      @return The 6532 for this console
    */
    M6532& riot() const { return *myRiot; }

    /**
      Saves the current state of this console class to the given Serializer.

      @param out The serializer device to save to.
      @return The result of the save.  True on success, false on failure.
    */
    bool save(Serializer& out) const override;

    /**
      Loads the current state of this console class from the given Serializer.

      @param in The Serializer device to load from.
      @return The result of the load.  True on success, false on failure.
    */
    bool load(Serializer& in) override;

    /**
      Set the properties to those given

      @param props The properties to use for the current game
    */
    void setProperties(const Properties& props);

    /**
      Query detailed information about this console.
    */
    const ConsoleInfo& about() const { return myConsoleInfo; }

    /**
      Timing information for this console.
    */
    ConsoleTiming timing() const { return myConsoleTiming; }

    /**
      Set up the console to use the debugger.
    */
    void attachDebugger(Debugger& dbg);

    /**
      Informs the Console of a change in EventHandler state.
    */
    void stateChanged(EventHandlerState state);

    /**
      Retrieve emulation timing provider.
     */
    EmulationTiming& emulationTiming() { return myEmulationTiming; }

  public:
    /**
      Toggle between NTSC/PAL/SECAM (and variants) display format.

      @param direction +1 indicates increase, -1 indicates decrease.
    */
    void toggleFormat(int direction = 1);

    /**
      Set NTSC/PAL/SECAM (and variants) display format.
    */
    void setFormat(uInt32 format);

    /**
      Get NTSC/PAL/SECAM (and variants) display format name
    */
    string getFormatString() const { return myDisplayFormat; }

    /**
      Toggle between the available palettes.
    */
    void togglePalette();

    /**
      Sets the palette according to the given palette name.

      @param palette  The palette to switch to.
    */
    void setPalette(const string& palette);

    /**
      Toggles phosphor effect.
    */
    void togglePhosphor();

    /**
      Change the "Display.PPBlend" variable.

      @param direction +1 indicates increase, -1 indicates decrease.
    */
    void changePhosphor(int direction);

    /**
      Toggles the PAL color-loss effect.
    */
    void toggleColorLoss();
    void enableColorLoss(bool state);

    /**
      Initialize the video subsystem wrt this class.
      This is required for changing window size, title, etc.

      @param full  Whether we want a full initialization,
                   or only reset certain attributes.

      @return  The results from FrameBuffer::initialize()
    */
    FBInitStatus initializeVideo(bool full = true);

    /**
      Initialize the audio subsystem wrt this class.
      This is required any time the sound settings change.
    */
    void initializeAudio();

    /**
      "Fry" the Atari (mangle memory/TIA contents)
    */
    void fry() const;

    /**
      Change the "Display.YStart" variable.

      @param direction +1 indicates increase, -1 indicates decrease.
    */
    void changeYStart(int direction);

    /**
      Returns the current framerate.
    */
    float getFramerate() const;

    /**
      Toggles the TIA bit specified in the method name.
    */
    void toggleP0Bit() const { toggleTIABit(P0Bit, "P0"); }
    void toggleP1Bit() const { toggleTIABit(P1Bit, "P1"); }
    void toggleM0Bit() const { toggleTIABit(M0Bit, "M0"); }
    void toggleM1Bit() const { toggleTIABit(M1Bit, "M1"); }
    void toggleBLBit() const { toggleTIABit(BLBit, "BL"); }
    void togglePFBit() const { toggleTIABit(PFBit, "PF"); }
    void toggleBits() const;

    /**
      Toggles the TIA collisions specified in the method name.
    */
    void toggleP0Collision() const { toggleTIACollision(P0Bit, "P0"); }
    void toggleP1Collision() const { toggleTIACollision(P1Bit, "P1"); }
    void toggleM0Collision() const { toggleTIACollision(M0Bit, "M0"); }
    void toggleM1Collision() const { toggleTIACollision(M1Bit, "M1"); }
    void toggleBLCollision() const { toggleTIACollision(BLBit, "BL"); }
    void togglePFCollision() const { toggleTIACollision(PFBit, "PF"); }
    void toggleCollisions() const;

    /**
      Toggles the TIA 'fixed debug colors' mode.
    */
    void toggleFixedColors() const;

    /**
      Toggles the TIA 'scanline jitter' mode.
    */
    void toggleJitter() const;

    /**
     * Update yatart and run autodetection if necessary.
     */
    void updateYStart(uInt32 ystart);

  private:
    /**
     * Dry-run the emulation and detect the frame layout (PAL / NTSC).
     */
    void autodetectFrameLayout(bool reset = true);

    /**
     * Dryrun the emulation and detect ystart (the first visible scanline).
     */
    void autodetectYStart(bool reset = true);

    /**
     * Rerun frame layout autodetection
     */
    void redetectFrameLayout();

    /**
     * Rerun ystart autodetection.
     */
    void redetectYStart();

    /**
      Sets various properties of the TIA (YStart, Height, etc) based on
      the current display format.
    */
    void setTIAProperties();

    /**
      Create the audio queue
     */
    void createAudioQueue();

    /**
      Adds the left and right controllers to the console.
    */
    void setControllers(const string& rommd5);

    /**
      Selects the left or right controller depending on ROM properties
    */
    unique_ptr<Controller> getControllerPort(const string& rommd5,
        const string& controllerName, Controller::Jack port);

    /**
      Loads a user-defined palette file (from OSystem::paletteFile), filling the
      appropriate user-defined palette arrays.
    */
    void loadUserPalette();

    /**
      Loads all defined palettes with PAL color-loss data, even those that
      normally can't have it enabled (NTSC), since it's also used for
      'greying out' the frame in the debugger.
    */
    void generateColorLossPalette();

    void toggleTIABit(TIABit bit, const string& bitname, bool show = true) const;
    void toggleTIACollision(TIABit bit, const string& bitname, bool show = true) const;

  private:
    // Reference to the osystem object
    OSystem& myOSystem;

    // Reference to the event object to use
    const Event& myEvent;

    // Properties for the game
    Properties myProperties;

    // Pointer to the 6502 based system being emulated
    unique_ptr<System> mySystem;

    // Pointer to the M6502 CPU
    unique_ptr<M6502> my6502;

    // Pointer to the 6532 (aka RIOT) (the debugger needs it)
    // A RIOT of my own! (...with apologies to The Clash...)
    unique_ptr<M6532> myRiot;

    // Pointer to the TIA object
    unique_ptr<TIA> myTIA;

    // The frame manager instance that is used during emulation
    unique_ptr<AbstractFrameManager> myFrameManager;

    // The audio fragment queue that connects TIA and audio driver
    shared_ptr<AudioQueue> myAudioQueue;

    // Pointer to the Cartridge (the debugger needs it)
    unique_ptr<Cartridge> myCart;

    // Pointer to the switches on the front of the console
    unique_ptr<Switches> mySwitches;

    // Pointers to the left and right controllers
    unique_ptr<Controller> myLeftControl, myRightControl;

    // Pointer to CompuMate handler (only used in CompuMate ROMs)
    shared_ptr<CompuMate> myCMHandler;

    // The currently defined display format (NTSC/PAL/SECAM)
    string myDisplayFormat;

    // Display format currently in use
    uInt32 myCurrentFormat;

    // Autodetected ystart.
    uInt32 myAutodetectedYstart;

    // Is ystart currently autodetected?
    bool myYStartAutodetected;

    // Is the TV format autodetected?
    bool myFormatAutodetected;

    // Indicates whether an external palette was found and
    // successfully loaded
    bool myUserPaletteDefined;

    // Contains detailed info about this console
    ConsoleInfo myConsoleInfo;

    // Contains timing information for this console
    ConsoleTiming myConsoleTiming;

    // Emulation timing provider. This ties together the timing of the core emulation loop
    // and the parameters that govern audio synthesis
    EmulationTiming myEmulationTiming;

    // The audio settings
    AudioSettings& myAudioSettings;

    // Table of RGB values for NTSC, PAL and SECAM
    static uInt32 ourNTSCPalette[256];
    static uInt32 ourPALPalette[256];
    static uInt32 ourSECAMPalette[256];

    // Table of RGB values for NTSC, PAL and SECAM - Z26 version
    static uInt32 ourNTSCPaletteZ26[256];
    static uInt32 ourPALPaletteZ26[256];
    static uInt32 ourSECAMPaletteZ26[256];

    // Table of RGB values for NTSC, PAL and SECAM - user-defined
    static uInt32 ourUserNTSCPalette[256];
    static uInt32 ourUserPALPalette[256];
    static uInt32 ourUserSECAMPalette[256];

  private:
    // Following constructors and assignment operators not supported
    Console() = delete;
    Console(const Console&) = delete;
    Console(Console&&) = delete;
    Console& operator=(const Console&) = delete;
    Console& operator=(Console&&) = delete;
};

#endif
