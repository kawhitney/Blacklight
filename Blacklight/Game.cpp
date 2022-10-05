/// \file Game.cpp
/// \brief Code for the game class CGame.

#include "Game.h"

#include "GameDefines.h"
#include "SpriteRenderer.h"
#include "ComponentIncludes.h"

#include "shellapi.h"

/// Delete the object manager. The renderer needs to be deleted before this
/// destructor runs so it will be done elsewhere.

CGame::~CGame() {
    delete m_pObjectManager;
} //destructor

/// Create the renderer and the object manager, load images and sounds, and
/// begin the game.

void CGame::Initialize() {
    m_pRenderer = new LSpriteRenderer(eSpriteMode::Batched2D);
    m_pRenderer->Initialize(eSprite::Size);
    LoadImages(); //load images from xml file list

    m_pObjectManager = new CObjectManager; //set up the object manager 
    LoadSounds(); //load the sounds for this game

    BeginGame();
} //Initialize

/// Load the specific images needed for this game. This is where `eSprite`
/// values from `GameDefines.h` get tied to the names of sprite tags in
/// `gamesettings.xml`. Those sprite tags contain the name of the corresponding
/// image file. If the image tag or the image file are missing, then the game
/// should abort from deeper in the Engine code leaving you with an error
/// message in a dialog box.

void CGame::LoadImages() {
    m_pRenderer->BeginResourceUpload();

    m_pRenderer->Load(eSprite::Background1, "background1");

    m_pRenderer->EndResourceUpload();
} //LoadImages

/// Initialize the audio player and load game sounds.

void CGame::LoadSounds() {
    m_pAudio->Initialize(eSound::Size);
    m_pAudio->Load(eSound::BackgroundSound1, "BackgroundSound1");
} //LoadSounds

/// Release all of the DirectX12 objects by deleting the renderer.

void CGame::Release() {
    delete m_pRenderer;
    m_pRenderer = nullptr; //for safety
} //Release

/// Ask the object manager to create the game objects. There's only one in this
/// game, the rotating wheel o' text centered at the center of the window.

void CGame::CreateObjects() {
    //m_pObjectManager->create(eSprite::TextWheel, m_vWinCenter);
} //CreateObjects

/// Call this function to start a new game. This should be re-entrant so that
/// you can restart a new game without having to shut down and restart the
/// program. All we need to do is delete any old objects out of the object
/// manager and create some new ones.

void CGame::BeginGame() {
    m_pObjectManager->clear(); //clear old objects
    CreateObjects(); //create new objects 
} //BeginGame

/// Poll the keyboard state and respond to the key presses that happened since
/// the last frame.

void CGame::KeyboardHandler() {
    m_pKeyboard->GetState(); //get current keyboard state 

    //if (m_pKeyboard->TriggerDown(VK_F1)) //help
        //ShellExecute(0, 0, "https://larc.unt.edu/code/blank/", 0, 0, SW_SHOW); -> Does not work rn

    if (m_pKeyboard->TriggerDown(VK_F2)) //toggle frame rate 
        m_bDrawFrameRate = !m_bDrawFrameRate;

    if (m_pKeyboard->TriggerDown(VK_BACK)) //restart game
        BeginGame(); //restart game
} //KeyboardHandler

/// Draw the current frame rate to a hard-coded position in the window.
/// The frame rate will be drawn in a hard-coded position using the font
/// specified in gamesettings.xml.

void CGame::DrawFrameRateText() {
    const std::string s = std::to_string(m_pTimer->GetFPS()) + " fps"; //frame rate
    const Vector2 pos(m_nWinWidth - 128.0f, 30.0f); //hard-coded position
    m_pRenderer->DrawScreenText(s.c_str(), pos); //draw to screen
} //DrawFrameRateText

/// Ask the object manager to draw the game objects. The renderer is notified
/// of the start and end of the frame so that it can let Direct3D do its
/// pipelining jiggery-pokery.

void CGame::RenderFrame() {
    m_pRenderer->BeginFrame(); //required before rendering

    m_pRenderer->Draw(eSprite::Background1, m_vWinCenter); //draw background
    m_pObjectManager->draw(); //draw objects
    if (m_bDrawFrameRate)DrawFrameRateText(); //draw frame rate, if required

    m_pRenderer->EndFrame(); //required after rendering
} //RenderFrame

/// This function will be called regularly to process and render a frame
/// of animation, which involves the following. Handle keyboard input.
/// Notify the  audio player at the start of each frame so that it can prevent
/// multiple copies of a sound from starting on the same frame.  
/// Move the game objects. Render a frame of animation.

void CGame::ProcessFrame() {
    KeyboardHandler(); //handle keyboard input
    m_pAudio->BeginFrame(); //notify audio player that frame has begun

    m_pTimer->Tick([&]() { //all time-dependent function calls should go here
        m_pObjectManager->move(); //move all objects
        });

    RenderFrame(); //render a frame of animation
} //ProcessFrame