//                                                              بسم اللہ الرحمان الرحیم  

#include <string>
#include <raylib.h>
using std::string;

/*
    14 Feb 25:
    Started bi-idhni-Allahi Taala

    TODO
        - powerups
        - transparent background img
        - 3d version
        - animations of "mr. pong :)"

    22 Feb 25: 
    Finished للہ الحمد کلہ :D
*/

enum GameState{
    // mainMenu
    MENU,          // main menu screen
    // PLAY,          // - play menu
    SETTINGS,      // - setting menu
    HELP,          // - help menu

    // gameOn
    PLAYING,       // game playing
    SCORE,         // menu after smone scores
    PAUSED,        // game paused
    GAMEOVER       // gameover 
};
enum LastScorer{
    PLAYER1,       // user / player1 in PvP & PvAI
    PLAYER2,       // player2 in PvP
    COMPUTER1,     // computer / ai in PvAI & AIvAI
    COMPUTER2      // computer2 in AIvAI
};
enum InputMode{
    WASD,
    ArrowKeys,
    Mouse
};
// enum GameMode{
//     PvP,
//     PvAI,
//     AIvAI
// };
enum Winner{
    FIRST,
    SECOND,
    NONE
};
enum Difficulty{
    EASY       = 8,             // these nums refers to the AIs base paddle velocity
    MEDIUM     = 9,
    HARD       = 10,
    BOSS       = 11
};
const Color transparentGold    = Color{255, 203, 0, 23};         // a lighter version of the defined GOLD color
const Color midTransparentGold = Color{255, 203, 0, 83};         

class Ball{
    private:
        const int radius;
        const int baseSpeed;
        int positionX;
        int positionY;
        int velocityX;
        int velocityY;
        const Color innerColor;
        const Color outerColor;

    public:
        Ball(const int radius, const int baseSpeed, const Color innerColor, const Color outerColor)
            : positionX(GetScreenWidth() / 2), positionY(GetScreenHeight() / 2), radius(radius), baseSpeed(baseSpeed), innerColor(innerColor), outerColor(outerColor){
            this->velocityX = (this->baseSpeed * (GetRandomValue(0, 1) ? 1 : -1));
            this->velocityY = (this->baseSpeed * (GetRandomValue(0, 1) ? 1 : -1));              // to randomize directions
        }

        void reset(){
            this->positionX = GetScreenWidth() / 2;
            this->positionY = GetScreenHeight() / 2;

            // to randomize directions
            this->velocityX = (this->baseSpeed * (GetRandomValue(0, 1)? 1 : -1));
            this->velocityY = (this->baseSpeed * (GetRandomValue(0, 1)? 1 : -1));
        }
        void update(){
            this->positionX += this->velocityX;
            if (((this->positionX - this->radius) <= 0) || ((this->positionX + this->radius) >= GetScreenWidth())){
                this->velocityX *= -1;                          // reversing component's direction vector
            }

            this->positionY += this->velocityY;
            if (((this->positionY - this->radius) <= 0) || ((this->positionY + this->radius) >= GetScreenHeight())){
                this->velocityY *= -1;
            }
        }
        void draw(){
            DrawCircleGradient(this->positionX, this->positionY, this->radius, this->innerColor, this->outerColor);
        }
        Vector2 getCenter(){
            return Vector2{(float) this->positionX, (float) this->positionY};
        }
        int getRadius(){
            return this->radius;
        }
        void reverseVelocityX(){
            this->velocityX *= -1;

            // to prevent the ball from being stuck inside the paddle
            this->positionX += ((this->velocityX > 0)? 35 : -35);
        }
};
class Paddle{
    protected:
        int positionX;
        int positionY;
        int velocityY;
        int width;
        int height;
        const int roundness;
        const Color color;
        Rectangle rectangle;
    

    public:
        Paddle(const int velocityY, const int width, const int height, const int roundness, const Color color)
            : width(width), height(height), roundness(roundness), velocityY(velocityY), color(color)
        {
            this->positionX = (5 + this->width / 2);                            // 5 = dist from edge of screen (ie offset)
            this->positionY = (GetScreenHeight() / 2 - this->height / 2);
            this->rectangle = Rectangle{(float) this->positionX, (float) this->positionY, (float) this->width, (float) this->height};
        }

        void update(InputMode playerInputMode){
            // move up
            switch (playerInputMode){
                case WASD:
                {
                    // move up
                    if (IsKeyDown(KEY_W)){
                        if ((positionY  - velocityY) >= 0){
                            positionY  -= velocityY;
                            rectangle.y = positionY;
                        }
                    }
                    // move down
                    else if (IsKeyDown(KEY_S)){
                        if (((positionY + height) + velocityY) <= GetScreenHeight()){
                            positionY  += velocityY;
                            rectangle.y = positionY;
                        }
                    }
                    break;
                }
                case ArrowKeys:
                {
                   // move up
                    if (IsKeyDown(KEY_UP)){
                        if ((positionY  - velocityY) >= 0){
                            positionY  -= velocityY;
                            rectangle.y = positionY;
                        }
                    }
                    // move down
                    else if (IsKeyDown(KEY_DOWN)){
                        if (((positionY + height) + velocityY) <= GetScreenHeight()){
                            positionY  += velocityY;
                            rectangle.y = positionY;
                        }
                    }
                    break;
                }
                case Mouse:
                {
                    // move up
                    if (GetMouseWheelMove() > 0){
                        if ((positionY  - velocityY) >= 0){
                            positionY  -= 1.5 * velocityY;            // 1.5x since mouse scrolling is slower
                            rectangle.y = positionY;
                        }
                    }
                    // move down
                    else if (GetMouseWheelMove() < 0){
                        if (((positionY + height) + velocityY) <= GetScreenHeight()){
                            positionY  += 1.5 * velocityY;
                            rectangle.y = positionY;
                        }
                    }
                    break;
                }
            }
        }
        void draw(){
            DrawRectangleRounded(this->rectangle, this->roundness, 1, this->color);
        }
        Rectangle getRectangle(){
            return this->rectangle;
        }
        void reset(){
            this->positionX = (5 + this->width / 2); // 5 = dist from edge of screen (ie offset)
            this->positionY = (GetScreenHeight() / 2 - this->height / 2);
            this->rectangle = Rectangle{(float) this->positionX, (float) this->positionY, (float) this->width, (float) this->height};
        }
};
class ComputerPaddle : public Paddle{
    public:
        ComputerPaddle(const int velocityY, const int width, const int height, const int roundness, const Color color)
            : Paddle(velocityY, width, height, roundness, color){
            this->positionX = (GetScreenWidth() - this->width - (5 + this->width / 2)); // 5 + width/2 = offset: ie dist from edge of screen
            this->rectangle.x = this->positionX;
        }

        void update(Vector2 ballCenter, int ballRadius){
            if ((ballCenter.x >= GetScreenWidth() / 2) && (ballCenter.y < this->positionY)){
                this->positionY -= this->velocityY;
                if (this->positionY < 0){ this->positionY = 0; }

                this->rectangle.y = this->positionY;
            }

            else if ((ballCenter.x >= GetScreenWidth() / 2) && (ballCenter.y > (this->positionY + this->height))){
                this->positionY += this->velocityY;
                if ((this->positionY + this->height) > GetScreenHeight()){ this->positionY = GetScreenHeight(); }

                this->rectangle.y = this->positionY;
            }
        }
        void reset(){
            this->positionX = (GetScreenWidth() - this->width - (5 + this->width / 2)); // 5 + width/2 = offset: ie dist from edge of screen
            this->positionY = (GetScreenHeight() / 2 - this->height / 2);
            this->rectangle = Rectangle{(float)this->positionX, (float)this->positionY, (float)this->width, (float)this->height};
        }
        void setAIDifficulty(Difficulty aiDifficulty){
            velocityY = aiDifficulty;
        }
};
// class PowerUp{
//     private:
//         int lastSpawnTime;

//     public:
//         PowerUp() {}
// };
class State{                                                        // abstract/base class to be inherited by all gameStates
    protected:
        GameState& gameState;

    public:
        State(GameState& gameState) : gameState(gameState) {}
        virtual ~State() = default;

        virtual void draw()   = 0;
        virtual void update() = 0;
};
class Menu : public State{
    private:
        string mainText         {"Mr. Pong :)"};
        int    mainTextSize     {140};
        int    buttonWidth      {500};
        int    buttonHeight     {100};
        int    playTextSize     {63};
        int    helpTextSize     {40};
        int    settingsTextSize {40};
        Sound  buttonClickSFX;

    public:
        Menu(GameState& gameState, const string& mainText, const int& mainTextSize, const int& buttonWidth, const int& buttonHeight, const int& playTextSize, const int& helpTextSize, const int& settingsTextSize) 
        : State(gameState), mainText(mainText), mainTextSize(mainTextSize),buttonWidth(buttonWidth), buttonHeight(buttonHeight), playTextSize(playTextSize), helpTextSize(helpTextSize), settingsTextSize(settingsTextSize) {}
        Menu(GameState& gameState) : State(gameState) {
            buttonClickSFX = LoadSound("Assets/SFX/buttonClick.mp3");
        }
        ~Menu(){
            UnloadSound(buttonClickSFX);
        }

        void draw(){
            // main Text
            DrawText(mainText.c_str(), GetScreenWidth()/2 - MeasureText(mainText.c_str(), mainTextSize)/2, GetScreenHeight()/5 - mainTextSize/2 + 50, mainTextSize, GOLD); 

            // play button
            DrawRectangleGradientH(GetScreenWidth()/2 - buttonWidth/2, GetScreenHeight()/2 - buttonHeight/2 + 50, buttonWidth, buttonHeight, GOLD, MAROON);
            DrawText("Play", GetScreenWidth()/2 - MeasureText("Play", playTextSize)/2, (GetScreenHeight()/2 - buttonHeight/2 + 50) + (buttonHeight - playTextSize)/2, playTextSize, BLACK);         // this took long...

            // help button
            DrawRectangleGradientH(GetScreenWidth()/2 - buttonWidth/2, (GetScreenHeight()/2 - buttonHeight/2 + 50) + buttonHeight + 23, buttonWidth / 2 - 23, buttonHeight - 23, GOLD, MAROON);
            DrawText("Help", (GetScreenWidth()/2 - buttonWidth/2) + (buttonWidth / 2 - 23 - MeasureText("Help", helpTextSize))/2, ((GetScreenHeight()/2 - buttonHeight/2 + 50) + buttonHeight + 23) + ((buttonHeight - 23) - helpTextSize)/2, helpTextSize, BLACK);

            // settings button; took too long...
            DrawRectangleGradientH(GetScreenWidth()/2 + 23, (GetScreenHeight()/2 - buttonHeight/2 + 50) + buttonHeight + 23, buttonWidth / 2 - 23, buttonHeight - 23, GOLD, MAROON);
            DrawText("Settings", (GetScreenWidth()/2 + 23) + (buttonWidth / 2 - 23 - MeasureText("Settings", settingsTextSize))/2, ((GetScreenHeight()/2 - buttonHeight/2 + 50) + buttonHeight + 23) + ((buttonHeight - 23) - settingsTextSize)/2, settingsTextSize, BLACK);
        }
        void update(){
            // play button
            if (CheckCollisionPointRec(GetMousePosition(), Rectangle{(float)(GetScreenWidth()/2 - buttonWidth/2), (float)(GetScreenHeight()/2 - buttonHeight/2 + 50), (float) buttonWidth, (float) buttonHeight})){
                playTextSize = 83;
                SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);     // no working :(
            
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    // gameState = PLAY;
                    gameState = PLAYING;
                    PlaySound(buttonClickSFX);
                    WaitTime(.5f);
                }
            }
            else{ 
                playTextSize = 63;
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }

            // help button
            if (CheckCollisionPointRec(GetMousePosition(), Rectangle{(float)(GetScreenWidth()/2 - buttonWidth/2), (float)((GetScreenHeight()/2 - buttonHeight/2 + 50) + buttonHeight + 23), (float)((buttonWidth) / 2 - 23), (float)(buttonHeight - 23)})){
                helpTextSize = 51;
                SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    gameState = HELP;
                    PlaySound(buttonClickSFX);
                }
            }
            else{ 
                helpTextSize = 40;
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }

            // settings button
            if (CheckCollisionPointRec(GetMousePosition(), Rectangle{(float)(GetScreenWidth()/2 + 23), (float)((GetScreenHeight()/2 - buttonHeight/2 + 50) + buttonHeight + 23), (float)(buttonWidth / 2 - 23), (float)(buttonHeight - 23)})){
                settingsTextSize = 51;
                SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    gameState = SETTINGS;
                    PlaySound(buttonClickSFX);
                }
            }
            else{ 
                settingsTextSize = 40;
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }
        }
};
// class Play : public State{
//     private:
//         GameMode gameMode {PvAI};

//     public:
//         Play(GameState& gameState) : State(gameState) {}

//         void draw(){

//         }
//         void update(){

//         }
// };
class Help : public State{
    public:
        Help(GameState& gameState) : State(gameState) {}

        void draw(){
            int titleSize {80};
            int textSize  {20};
            int padding   {50};

            DrawText("Help", GetScreenWidth()/2 - MeasureText("Help", titleSize)/2, padding - 20, titleSize, GOLD);

            // controls section
            int y {140};
            DrawText("Controls", padding, y, 40, GOLD); y += 60;
            DrawText("Move Up      -  W  or  Arrow Up",   padding, y, textSize, RAYWHITE); y += 40;
            DrawText("Move Down    -  S  or  Arrow Down", padding, y, textSize, RAYWHITE); y += 40;
            DrawText("Pause        -  P",                 padding, y, textSize, RAYWHITE); y += 50;

            // objective section
            DrawText("Objective", padding, y, 40, GOLD); y += 60;
            DrawText("Hit the ball past the opponent's paddle to score.", padding, y, textSize, RAYWHITE); y += 40;
            DrawText("First to 7 points wins.",                           padding, y, textSize, RAYWHITE); y += 50;

            // modes section
            DrawText("Game Modes", padding, y, 40, GOLD); y += 60;
            DrawText("PvAI   -  You vs Computer",       padding, y, textSize, RAYWHITE); y += 40;
            DrawText("PvP    -  You vs a Friend",       padding, y, textSize, RAYWHITE); y += 40;
            DrawText("AIvAI  -  Watch two AIs battle",  padding, y, textSize, RAYWHITE); y += 40;

            DrawText("Press ENTER to go back", GetScreenWidth() - 10 - MeasureText("Press ENTER to go back", 30), GetScreenHeight() - 40, 30, midTransparentGold);
        }
        void update(){
            if (IsKeyPressed(KEY_ENTER)) gameState = MENU;
        }
};
class Settings : public State{
    private:
        bool       fullScreen      {false};         // setwindowstate(FLAG_BORDERLESS_WINDOWED_MODE); ClearWindowState(); SetWindowSize();
        int        frameRate       {63};
        float      windowOpacity   {0.8f};
        float      masterVolume    {0.5};
        InputMode  playerInputMode {WASD};
        Difficulty aiDifficulty    {EASY};

        const int    posX     {23};
        int          posY     {63};
        const int    textSize {35};
        const int    offSet   {100};             // gap between text/heading and option/toggler
        const Color  color    {GOLD};
        const string texts[6] {"   > FullScreen     ", "   > FrameRate     ", "   > Window Opacity", "   > SFX Volume    ", "   > Input Mode    ", "   > AI Difficulty   "};

        Sound settingModifySFX;

    public:
        Settings(GameState& gameState) : State(gameState) {
            settingModifySFX = LoadSound("Assets/SFX/settingModify.mp3");
        }
        ~Settings(){
            UnloadSound(settingModifySFX);
        }

        void draw(){
            posY = 23;

            // header
            DrawText("Settings", posX, posY, 53, GOLD);

            // instructions
            // DrawText("Click on a setting or scroll through it to change it.", GetScreenWidth() - MeasureText("Click on a setting or scroll through it to change it.", 23), 35, 23, GOLD);
            DrawText("Press ENTER to go back.", GetScreenWidth() - MeasureText("Press ENTER to go back.", 23) - 23, GetScreenHeight() - 50, 23, GOLD);

            // fullscreen
            posY += (offSet + ((fullScreen)? 63 : 0));
            DrawText(texts[0].c_str(), posX, posY, textSize, color);
            DrawText(TextFormat("%s", (fullScreen)? "Enabled" : "Disabled"), posX + MeasureText(texts[0].c_str(), textSize) + 100, posY, textSize, (fullScreen)? GREEN : RED);

            // frame rate

            posY += offSet;
            DrawText(texts[1].c_str(), posX, posY, textSize, color);
            DrawText(TextFormat("%d", frameRate), posX + MeasureText(texts[1].c_str(), textSize) + 100, posY, textSize, (frameRate == 23)? RED : (frameRate == 40)? ORANGE : (frameRate == 63)? YELLOW : GREEN);
            
            // window opacity
            posY += offSet;
            DrawText(texts[2].c_str(), posX, posY, textSize, color);
            DrawText(TextFormat("%.1f", windowOpacity), posX + MeasureText(texts[2].c_str(), textSize) + offSet, posY, textSize, (windowOpacity <= 0.4)? RED : (windowOpacity <= 0.7)? YELLOW : GREEN);
            
            // master volume
            posY += offSet;
            DrawText(texts[3].c_str(), posX, posY, textSize, color);
            DrawText(TextFormat("%.0f%%", masterVolume * 100), posX + MeasureText(texts[3].c_str(), textSize) + offSet, posY, textSize, (masterVolume <= 0.4)? RED : (masterVolume <= 0.7)? YELLOW : GREEN);

            // input mode
            posY += offSet;
            DrawText(texts[4].c_str(), posX, posY, textSize, color);
            // (playerInputMode == WASD)? BLUE : (playerInputMode == ArrowKeys)? PURPLE : BEIGE);      
            DrawText(TextFormat("%s", (playerInputMode == WASD)? "WASD Keys" : (playerInputMode == ArrowKeys)? "Arrow Keys" : "Mouse Wheel"), posX + MeasureText(texts[4].c_str(), textSize) + offSet, posY, textSize, BEIGE);  

            // ai difficulty
            posY += offSet;
            DrawText(texts[5].c_str(), posX, posY, textSize, color);
            DrawText(TextFormat("%s", (aiDifficulty == EASY)? "Easy" : (aiDifficulty == MEDIUM)? "Medium" : (aiDifficulty == HARD)? "Hard" : "Boss"), posX + MeasureText(texts[5].c_str(), textSize) + offSet, posY, textSize, (aiDifficulty == EASY)? GREEN : (aiDifficulty == MEDIUM)? YELLOW : (aiDifficulty == HARD)? RED : DARKPURPLE);      // or beige

            // DrawText("There supposed to be some stuff hee...", GetScreenWidth()/2 - MeasureText("There supposed to be some stuff here...", 40)/2, GetScreenHeight()/2, 40, midTransparentGold);

            // in prog
            // DrawRectangle((GetScreenWidth() - GetScreenHeight()/1.5)/2, GetScreenHeight()/4, GetScreenWidth()/1.5, GetScreenHeight()/2, transparentGold);
            // DrawText("Press ENTER to go back", 5, GetScreenHeight() - 300 / 8 - 5, 300 / 8, midTransparentGold);
        }

        void update(){
            if (IsKeyPressed(KEY_ENTER)){
                gameState = MENU;
            }

            posY = 23;

            // fullscreen
            posY += (offSet + ((fullScreen)? 63 : 0));
            if (CheckCollisionPointRec(GetMousePosition(), Rectangle{(float) posX + MeasureText(texts[0].c_str(), textSize) + offSet, (float) posY, (float) MeasureText("Disabled", textSize), (float) textSize}) && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || GetMouseWheelMove()))
            {
                fullScreen = !fullScreen;
                if (fullScreen){
                    // SetWindowState(FLAG_FULLSCREEN_MODE);
                    SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE); // op :D
                }
                else{
                    ClearWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
                    SetWindowSize(1300, 700);
                }

                PlaySound(settingModifySFX);
            }

            // framerate
            posY += offSet;
            if (CheckCollisionPointRec(GetMousePosition(), Rectangle{(float) posX + MeasureText(texts[1].c_str(), textSize) + offSet, (float) posY, (float) MeasureText("123", textSize), (float) textSize}) && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || GetMouseWheelMove()))          // 123 is the max width of poss values
            {
                frameRate = ((frameRate == 23)? 40 : (frameRate == 40)? 63 : (frameRate == 63)? 123 : 23);          // y these values? simple: me like em :)
                
                SetTargetFPS(frameRate);
                PlaySound(settingModifySFX);
            }

            // window opacity
            posY += offSet;
            if (CheckCollisionPointRec(GetMousePosition(), Rectangle{(float) posX + MeasureText(texts[2].c_str(), textSize) + offSet, (float) posY, (float) MeasureText("0.1", textSize), (float) textSize}) && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || GetMouseWheelMove()))          // 0.1 is the max width of poss values
            {
                windowOpacity += 0.1f;
                if (windowOpacity >= 1.1f){             // >= needed since float addition can result in smthing like 1.00001f (wahi fpn kay precision ka masla)
                    windowOpacity = 0.1f;
                }
                
                SetWindowOpacity(windowOpacity);
                PlaySound(settingModifySFX);
            }

            // master volume
            posY += offSet;
            if (CheckCollisionPointRec(GetMousePosition(), Rectangle{(float) posX + MeasureText(texts[3].c_str(), textSize) + offSet, (float) posY, (float) MeasureText("100%", textSize), (float) textSize}) && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || GetMouseWheelMove()))          // 100% is the max width of poss values
            {
                masterVolume += 0.1f;
                if (masterVolume >= 1.1f){             // >= needed since float addition can result in smthing like 1.00001f (wahi fpn kay precision ka masla)
                    masterVolume = 0.0f;
                }
                
                SetMasterVolume(masterVolume);
                PlaySound(settingModifySFX);             // to test modified vol
            }

            // input mode
            posY += offSet;
            if (CheckCollisionPointRec(GetMousePosition(), Rectangle{(float) posX + MeasureText(texts[4].c_str(), textSize) + offSet, (float) posY, (float) MeasureText("Mouse Wheel", textSize), (float) textSize}) && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || GetMouseWheelMove()))          // "Mouse Wheel" is the max width of poss values
            {
                playerInputMode = ((playerInputMode == WASD)? ArrowKeys : (playerInputMode == ArrowKeys)? Mouse : WASD);
                
                PlaySound(settingModifySFX);
            }

            // ai difficulty
            posY += offSet;
            if (CheckCollisionPointRec(GetMousePosition(), Rectangle{(float) posX + MeasureText(texts[5].c_str(), textSize) + offSet, (float) posY, (float) MeasureText("Medium", textSize), (float) textSize}) && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || GetMouseWheelMove()))          // "medium" is the max width of poss values
            {
                aiDifficulty = ((aiDifficulty == EASY)? MEDIUM : (aiDifficulty == MEDIUM)? HARD : (aiDifficulty == HARD)? BOSS : EASY);
                
                PlaySound(settingModifySFX);
            }
        }
        InputMode getPlayerInputMode(){
            return playerInputMode;
        }
        Difficulty getAIDifficulty(){
            return aiDifficulty;
        }
};
class Playing : public State{
    private:        
        Settings& settings;

        const int ballBaseSpeed           {11};
        const int paddleVelocityY         {12};
        const int computerPaddleVelocityY {10};
        
        Ball           ball;
        Paddle         player;
        ComputerPaddle computer;
        
        int         playerScore    {0};
        int         computerScore  {0};
        LastScorer  lastScorer     {};
        Winner      winner         {NONE};
        bool        roundStart     {true};
        bool        newGame        {true};
        
        int         pauseStartTime {0};
        const int   pauseDuration  {3};
        
        const int   fontSize       {300};
        const Color color          {255, 203, 0, 40};
        
        Sound roundStartSFX;
        Sound ballHitSFX;
        Sound playerScoreSFX;
        Sound computerScoreSFX;
        Sound gamePausedSFX;
        Sound gameOverSFX;

        double elapsedTime;
        int    gameTime;

        void reset(){
            ball.reset();
            player.reset();
            computer.reset();
        }

    public:
        Playing(GameState& gameState, Settings& settings)
        :   State(gameState),
            settings(settings),
            ball(35, ballBaseSpeed, GOLD, MAROON),             // the more diff the ai/computerpaddle, the greater its velocityY
            player(paddleVelocityY, 23, 135, 50, GOLD),
            computer(settings.getAIDifficulty(), 23, 135, 50, GOLD)
        {
            roundStartSFX    = LoadSound("Assets/SFX/roundStart.mp3"); 
            ballHitSFX       = LoadSound("Assets/SFX/ballHit.mp3");
            playerScoreSFX   = LoadSound("Assets/SFX/playerScore.mp3");
            computerScoreSFX = LoadSound("Assets/SFX/computerScore.mp3");
            gamePausedSFX    = LoadSound("Assets/SFX/gamePaused.mp3");
            gameOverSFX      = LoadSound("Assets/SFX/gameOver.mp3");
        }
        ~Playing(){
            UnloadSound(roundStartSFX);
            UnloadSound(ballHitSFX);
            UnloadSound(playerScoreSFX);
            UnloadSound(computerScoreSFX);
            UnloadSound(gamePausedSFX);
            UnloadSound(gameOverSFX);
        }

        void draw(){
            // divider
            DrawRectangle(GetScreenWidth()/2 - 5, 0, 10, GetScreenHeight()/2 - GetScreenWidth()/7, color);
            DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, GetScreenWidth()/7, color);
            DrawRectangle(GetScreenWidth()/2 - 5, GetScreenHeight()/2 + GetScreenWidth()/7, 10, GetScreenHeight(), color);

            // player identifiers
            DrawText("You", 10, 10, 50, color);
            DrawText("AI", GetScreenWidth() - 10 - MeasureText("AI", 50), 5, 50, color);

            // scores
            DrawText(TextFormat("%d", playerScore),       GetScreenWidth()/4   - MeasureText(TextFormat("%d", playerScore),   fontSize)/2, GetScreenHeight()/2 - fontSize/2, fontSize, color);
            DrawText(TextFormat("%d", computerScore), 3 * GetScreenWidth()/4   - MeasureText(TextFormat("%d", computerScore), fontSize)/2, GetScreenHeight()/2 - fontSize/2, fontSize, color);

            // elapsed time
            // double elapsedTime {GetTime()};
            // DrawText(TextFormat("%d", (int) elapsedTime), GetScreenWidth() / 2 - MeasureText(TextFormat("%d", (int) elapsedTime), fontSize / 1.5) / 2, GetScreenHeight() / 2 - fontSize / 3, fontSize / 1.5, BLACK);
            DrawText(TextFormat("%d", gameTime), GetScreenWidth() / 2 - MeasureText(TextFormat("%d", gameTime), fontSize / 1.5) / 2, GetScreenHeight() / 2 - fontSize / 3, fontSize / 1.5, BLACK);
            
            ball.draw();
            player.draw();
            computer.draw();
        }
        void update(){
            if (newGame){
                elapsedTime = GetTime();
                gameTime = 0;
                newGame = false;
                computer.setAIDifficulty(settings.getAIDifficulty());
            }
            if (roundStart){
                PlaySound(roundStartSFX);
                roundStart = false;
                WaitTime(1);
            }
            if ((GetTime() - elapsedTime) >= 1.0f){
                gameTime++;
                elapsedTime = GetTime();
            }

            ball.update();
            player.update(settings.getPlayerInputMode());
            computer.update(ball.getCenter(), ball.getRadius());

            // collisions
            if (CheckCollisionCircleRec(ball.getCenter(), ball.getRadius(), player.getRectangle())){
                ball.reverseVelocityX();
                PlaySound(ballHitSFX);
            }
            if (CheckCollisionCircleRec(ball.getCenter(), ball.getRadius(), computer.getRectangle())){
                ball.reverseVelocityX();
                PlaySound(ballHitSFX);
            }

            // scoring
            if ((ball.getCenter().x + ball.getRadius()) >= GetScreenWidth()){
                playerScore++;
                PlaySound(playerScoreSFX);
                lastScorer = PLAYER1;
                reset();
                gameState = SCORE;
                pauseStartTime = GetTime();
                roundStart = true;
            }
            if ((ball.getCenter().x - ball.getRadius()) <= 0){
                computerScore++;
                PlaySound(computerScoreSFX);
                lastScorer = COMPUTER1;
                reset();
                gameState = SCORE;
                pauseStartTime = GetTime();
                roundStart = true;
            }

            if (playerScore == 7 || computerScore == 7){
                gameState = GAMEOVER;
                winner = ((playerScore > computerScore)? FIRST : (playerScore == computerScore)? NONE : SECOND);
                
                reset();
                PlaySound(gameOverSFX);
            }
            if ((gameState == PLAYING) && (!IsWindowFocused() || IsKeyPressed(KEY_P))){
                gameState = PAUSED;
                PlaySound(gamePausedSFX);
            }
        }

        int        getPlayerScore()   { return playerScore;                 }
        int        getComputerScore() { return computerScore;               }
        LastScorer getLastScorer()    { return lastScorer;                  }
        int        getPauseStart()    { return pauseStartTime;              }
        void       resetScores()      { playerScore = 0; computerScore = 0; }
        void       resetPauseStart()  { pauseStartTime = 0;                 }
        Winner     getWinner()        { return winner;                      }
        void       resetAll()         { 
            reset(); 
            resetScores(); 
            resetPauseStart();
            roundStart = true;
            newGame = true;
            gameTime = 0;
        }
};
class Score : public State{
    private:
        Playing& playing;
        const int textSize {83};
        const int fontSize {300};

    public:
        Score(GameState& gameState, Playing& playing) : State(gameState), playing(playing) {}

        void draw(){
            // 'who scored' text
            DrawText(TextFormat("%s Scored!", (playing.getLastScorer() == PLAYER1)? "You" : "Computer"), GetScreenWidth() / 2 - MeasureText(TextFormat("%s Scored!", (playing.getLastScorer() == PLAYER1)? "You" : "Computer"), textSize)/2, GetScreenHeight()/2 - textSize/2, textSize, midTransparentGold);        // thats long....;  100 here is the fontsize
            
            // 'resuming in' text
            DrawText(TextFormat("(Game Resuming in %d...)", 3 - ((int) GetTime() - playing.getPauseStart())), 5, GetScreenHeight() - fontSize / 8 - 5, fontSize / 8, midTransparentGold);
        }
        void update(){
            if (((int) GetTime() - playing.getPauseStart()) >= 3){
                gameState = PLAYING;
                playing.resetPauseStart();
            }
        }
};
class Paused : public State{
    private:
        Playing& playing;
        int      textSize {83};
        Sound    gameResumedSFX;
        Sound    gameStoppedSFX;

    public:
        Paused(GameState& gameState, Playing& playing) : State(gameState), playing(playing) {
            gameResumedSFX = LoadSound("Assets/SFX/gameResumed.mp3");
            gameStoppedSFX = LoadSound("Assets/SFX/gameStopped.mp3");
        }
        ~Paused(){
            UnloadSound(gameResumedSFX);
            UnloadSound(gameStoppedSFX);
        }

        void draw(){
            // DrawText("GamePlay Paused.\n- Press \"p\" to resume.\n- Press \"x\" to finish.", GetScreenWidth() / 2 - MeasureText("GamePlay Paused.\n- Press \"p\" to resume.\n- Press \"x\" to finish.", textSize) / 2, GetScreenHeight() /2 - 3*textSize/2, textSize, GOLD);        // thats long....;  100 here is the fontsize
            DrawText("GamePlay Paused.", GetScreenWidth() / 2 - MeasureText("GamePlay Paused.", textSize) / 2, GetScreenHeight()/2 - textSize/2 - 50, textSize, GOLD);        // thats long....;  100 here is the fontsize
            DrawText("- Press \"p\" to resume.\n- Press \"x\" to finish.", GetScreenWidth() / 2 - MeasureText("- Press \"p\" to resume.\n- Press \"x\" to finish.", textSize/1.5) / 2, GetScreenHeight()/2 - textSize/2 - 50 + textSize + 30, textSize/1.5, midTransparentGold);
        }
        void update(){
            if (IsKeyPressed(KEY_P)){
                PlaySound(gameResumedSFX);

                gameState = PLAYING;
                WaitTime(1);
            }
            if (IsKeyPressed(KEY_X)){
                PlaySound(gameStoppedSFX);
                
                gameState = MENU;
                playing.resetAll();
                WaitTime(1);
            }
        }
};
class GameOver : public State{
    private:
        Playing&    playing;
        const int   fontSize    {300};
        const int   waitTime    {10};
        int         arrivalTime {0};
        bool        arrived     {false};

    public:
        GameOver(GameState& gameState, Playing& playing) : State(gameState), playing(playing) {}

        void draw(){
            if (!arrived){ arrivalTime = GetTime(); arrived = true; }

            // winner text
            string winnerText;
            switch(playing.getWinner()){
                case FIRST:  winnerText = "You Win! :D";    break;
                case SECOND: winnerText = "Computer Wins."; break;
                default:     winnerText = "It's a Draw!";   break;
            }
            int textSize {120};
            DrawText(winnerText.c_str(), GetScreenWidth()/2 - MeasureText(winnerText.c_str(), textSize)/2, GetScreenHeight()/2 - textSize, textSize, GOLD);

            // scores
            const char* scoreText = TextFormat("%d  -  %d", playing.getPlayerScore(), playing.getComputerScore());
            DrawText(scoreText, GetScreenWidth()/2 - MeasureText(scoreText, 80)/2, GetScreenHeight()/2 + 20, 80, midTransparentGold);

            // returning to menu countdown
            int remaining = waitTime - ((int)GetTime() - arrivalTime);
            DrawText(TextFormat("Returning to menu in %d...  or press ENTER", remaining), 5, GetScreenHeight() - fontSize/8 - 5, fontSize/8, midTransparentGold);
        }
        void update(){
            if (!arrived) return;

            if (IsKeyPressed(KEY_ENTER) || ((int) GetTime() - arrivalTime) >= waitTime){
                playing.resetAll();
                arrived = false;
    
                gameState = MENU;
            }
        }
};
struct SFX{
    // Sound buttonClick;
    // Sound someButtonHoverSound;
    Sound ballHit;
    // Sound powerUp;
    Sound playerScore;
    Sound computerScore;
    // Sound paused;
    // Sound wait;
    // Sound gameOver;
};

class Game{
    private:
        GameState gameState {MENU};

        Menu     menu       {gameState          };
        // Play     play       {gameState};
        Help     help       {gameState          };
        Settings settings   {gameState          };
        Playing  playing    {gameState, settings};
        Paused   paused     {gameState, playing };
        Score    score      {gameState, playing };
        GameOver gameOver   {gameState, playing };

    public:
        Game(){}

        void draw(){
            // clearing background
            ClearBackground(BLANK);

            switch(gameState)
            {   
                case MENU:     { menu.draw();     break; }
                // case PLAY:     { play.draw();     break; }
                case SETTINGS: { settings.draw(); break; }
                case HELP:     { help.draw();     break; }

                case PLAYING:  { playing.draw();  break; }
                case SCORE:    { score.draw();    break; }
                case PAUSED:   { paused.draw();   break; }
                case GAMEOVER: { gameOver.draw(); break; }
            }
        }
        void update(){
            switch(gameState)
            {
                case MENU:     { menu.update();     break; }
                // case PLAY:     { play.update();     break; }
                case SETTINGS: { settings.update(); break; }
                case HELP:     { help.update();     break; }

                case PLAYING:  { playing.update();  break; }
                case SCORE:    { score.update();    break; }
                case PAUSED:   { paused.update();   break; } 
                case GAMEOVER: { gameOver.update(); break; }
            }
        }
        void end(){
            // me too farigh :) alhamdulillah

            Sound windowCloseSFX = LoadSound("Assets/SFX/windowClose.mp3");
            PlaySound(windowCloseSFX);          // :D

            BeginDrawing();
                ClearBackground(BLANK);
                DrawText("Plz don't leave meeeee :(", GetScreenWidth()/2 - MeasureText("Plz don't leave meeeee :(", 63)/2, GetScreenHeight()/2 - 63/2, 63, GOLD);
                DrawText("Made by Saad, bi-idhni-Allahi Taala :D", 23, GetScreenHeight() - 35 - 5, 35, midTransparentGold);
            EndDrawing();

            WaitTime(2.5);
            UnloadSound(windowCloseSFX);
        }
};

int main()
{
    const int screenWidth  {1300};
    const int screenHeight  {700};

    InitWindow(screenWidth, screenHeight, "Mr. Pong 🏓");
    SetWindowOpacity(0.8);
    SetTargetFPS(63);
    SetExitKey(KEY_ESCAPE);
    InitAudioDevice();

    Image icon = LoadImage("Assets/Favicon/2.png");
    if (icon.data){
        SetWindowIcon(icon);
        UnloadImage(icon);
    }


    Game game;

    // gameLoop
    while (!WindowShouldClose()){
        // update
        game.update();        

        // draw
        BeginDrawing();

            game.draw();

        EndDrawing();
    }

    game.end();

    CloseAudioDevice();
    CloseWindow();
} 