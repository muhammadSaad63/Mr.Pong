#include <string>
#include <raylib.h>
using std::string;

/*
    TODO
        - powerups
        - win condition
        - replay/regame/reset
        - transparent background img
        - 3d version
        - animations of "mr. pong :)"
        - revamp Playing class with all stuff (paddles, ball, time, etc)
        - make a base/abstract class with priv gamestate and public constructor, draw(), and update() which inherited by all
        - make every gameState class inherit class State
*/

enum GameState{
    // mainMenu
    MENU,          // main menu screen
    PLAY,          // - play menu
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
enum GameMode{
    PvP,
    PvAI,
    AIvAI
};
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
            this->velocityY = (this->baseSpeed * (GetRandomValue(0, 1) ? 1 : -1)); // to randomize directions
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

        void update(){
            // move up
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)){
                if ((this->positionY  - this->velocityY) >= 0){
                    this->positionY  -= this->velocityY;
                    this->rectangle.y = this->positionY;
                }
            }

            // move down
            else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)){
                if (((this->positionY + this->height) + this->velocityY) <= GetScreenHeight()){
                    this->positionY  += this->velocityY;
                    this->rectangle.y = this->positionY;
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
};
class PowerUp{
    private:
        int lastSpawnTime;
};
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
        string mainText         {"Mr. Pong :D"};
        int    mainTextSize     {140};
        int    buttonWidth      {500};
        int    buttonHeight     {100};
        int    playTextSize     {63};
        int    helpTextSize     {40};
        int    settingsTextSize {40};

    public:
        Menu(GameState& gameState, const string& mainText, const int& mainTextSize, const int& buttonWidth, const int& buttonHeight, const int& playTextSize, const int& helpTextSize, const int& settingsTextSize) 
        : State(gameState), mainText(mainText), mainTextSize(mainTextSize),buttonWidth(buttonWidth), buttonHeight(buttonHeight), playTextSize(playTextSize), helpTextSize(helpTextSize), settingsTextSize(settingsTextSize) {}
        Menu(GameState& gameState) : State(gameState) {}

        void draw(){
            // main Text
            DrawText(mainText.c_str(), GetScreenWidth()/2 - MeasureText(mainText.c_str(), mainTextSize)/2, GetScreenHeight()/5 - mainTextSize/2, mainTextSize, GOLD); 

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
            
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                    gameState = PLAY;
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
            
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                    gameState = HELP;
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
            
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                    gameState = SETTINGS;
                }
            }
            else{ 
                settingsTextSize = 40;
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }
        }
};
class Play : public State{
    private:
        GameMode gameMode;

    public:
        Play(GameState& gameState) : State(gameState) {}

        void draw(){

        }
        void update(){

        }
};
class Help : public State{
    private:

    public:
        Help(GameState& gameState) : State(gameState) {}

        void draw(){
            DrawText("Help", 23, 23, 80, GOLD);

            // in prog
            DrawRectangle((GetScreenWidth() - GetScreenHeight()/1.5)/2, GetScreenHeight()/4, GetScreenWidth()/1.5, GetScreenHeight()/2, Color{255, 203, 0, 23});
            DrawText("Press \"Enter\" to go back", 5, GetScreenHeight() - 300 / 8 - 5, 300 / 8, Color{255, 203, 0, 83});
        }

        void update(){
            if (IsKeyPressed(KEY_ENTER)){
                gameState = MENU;
            }
        }
};
class Settings : public State{
    private:
        // float     windowOpacity {0.8f};
        // int       frameRate     {63};
        // int       masterVolume  {0.5};
        // bool      muteSounds    {false};
        // InputMode player1Input  {WASD};
        // InputMode player2Input  {ArrowKeys};
        // bool      fullScreen    {false};         // setwindowstate(FLAG_BORDERLESS_WINDOWED_MODE); ClearWindowState(); SetWindowSize();


    public:
        Settings(GameState& gameState) : State(gameState) {}

        void draw(){
            DrawText("Settings", 23, 23, 50, GOLD);

            // in prog
            DrawRectangle((GetScreenWidth() - GetScreenHeight()/1.5)/2, GetScreenHeight()/4, GetScreenWidth()/1.5, GetScreenHeight()/2, Color{255, 203, 0, 23});
            DrawText("Press \"Enter\" to go back", 5, GetScreenHeight() - 300 / 8 - 5, 300 / 8, Color{255, 203, 0, 83});
        }

        void update(){
            if (IsKeyPressed(KEY_ENTER)){
                gameState = MENU;
            }
        }
};
class Playing : public State{
    private:        
        Ball           ball;
        Paddle         player;
        ComputerPaddle computer;
        
        int        playerScore    {0};
        int        computerScore  {0};
        LastScorer lastScorer;
        
        int pauseStartTime  {0};
        const int pauseDuration {3};
        
        const Color color;
        const int   fontSize {300};
        
        Sound ballHitSFX;
        Sound playerScoreSFX;
        Sound computerScoreSFX;

        void reset(){
            ball.reset();
            player.reset();
            computer.reset();
        }

    public:
        Playing(GameState& gameState, const Color color)
        :   State(gameState), color(color),
            ball(35, 10, GOLD, MAROON),
            player(11, 23, 135, 50, GOLD),
            computer(9, 23, 135, 50, GOLD)
        {
            ballHitSFX       = LoadSound("Assets/SFX/ballHit.mp3");
            playerScoreSFX   = LoadSound("Assets/SFX/playerScore.mp3");
            computerScoreSFX = LoadSound("Assets/SFX/computerScore.mp3");
        }
        ~Playing(){
            UnloadSound(ballHitSFX);
            UnloadSound(playerScoreSFX);
            UnloadSound(computerScoreSFX);
        }

        void draw(){
            // divider
            DrawRectangle(GetScreenWidth()/2 - 5, 0, 10, GetScreenHeight()/2 - GetScreenWidth()/7, color);
            DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, GetScreenWidth()/7, color);
            DrawRectangle(GetScreenWidth()/2 - 5, GetScreenHeight()/2 + GetScreenWidth()/7, 10, GetScreenHeight(), color);

            // scores
            DrawText(TextFormat("%d", playerScore),       GetScreenWidth()/4   - MeasureText(TextFormat("%d", playerScore),   fontSize)/2, GetScreenHeight()/2 - fontSize/2, fontSize, color);
            DrawText(TextFormat("%d", computerScore), 3 * GetScreenWidth()/4   - MeasureText(TextFormat("%d", computerScore), fontSize)/2, GetScreenHeight()/2 - fontSize/2, fontSize, color);

            // elapsed time
            double elapsedTime {GetTime()};
            DrawText(TextFormat("%d", (int) elapsedTime), GetScreenWidth() / 2 - MeasureText(TextFormat("%d", (int) elapsedTime), fontSize / 1.5) / 2, GetScreenHeight() / 2 - fontSize / 3, fontSize / 1.5, BLACK);
        
            ball.draw();
            player.draw();
            computer.draw();
        }
        void update(){
            ball.update();
            player.update();
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
            }
            if ((ball.getCenter().x - ball.getRadius()) <= 0){
                computerScore++;
                PlaySound(computerScoreSFX);
                lastScorer = COMPUTER1;
                reset();
                gameState = SCORE;
                pauseStartTime = GetTime();
            }

            if (IsKeyPressed(KEY_P)){
                gameState = PAUSED;
            }
        }

        // called by score screen
        LastScorer getLastScorer()    { return lastScorer; }
        int        getPlayerScore()   { return playerScore; }
        int        getComputerScore() { return computerScore; }
        int        getPauseStart()    { return pauseStartTime; }
};
class Score : public State{
    private:
        Playing playing;

    public:
        Score(GameState& gameState, Playing& playing) : State(gameState), playing(playing) {}

        void draw(){
            // 'who scored' text
            const int textSize {83};
            DrawText(TextFormat("%s Scored!", (playing.getLastScorer() == PLAYER1)? "You" : "Computer"), GetScreenWidth() / 2 - MeasureText(TextFormat("%s Scored!", (this->lastScorer == PLAYER1)? "You" : "Computer"), textSize)/2, GetScreenHeight()/2 - textSize/2, textSize, Color{this->color.r, this->color.g, this->color.b, 83});        // thats long....;  100 here is the fontsize
            
            // 'resuming in' text
            DrawText(TextFormat("(Game Resuming in %d...)", playing.g - ((int) GetTime() - playing.getPauseStart())), 5, GetScreenHeight() - this->fontSize / 8 - 5, this->fontSize / 8, Color{this->color.r, this->color.g, this->color.b, 83});
        }
        void update(){
            if (((int) GetTime() - playing.getPauseStart()) >= 3){
                this->gameState = PLAYING;
                // playing.getPauseStart() = 0;
            }
        }
};
class Paused : public State{
    private:
        int textSize {83};

    public:
        Paused(GameState& gameState) : State(gameState) {}

        void draw(){
            DrawText("GamePlay Paused.\nPress \"p\" to resume.", GetScreenWidth() / 2 - MeasureText("GamePlay Paused.\nPress 'p' to resume.", textSize) / 2, GetScreenHeight() / 2 - textSize, textSize, Color{255, 203, 0, 83});        // thats long....;  100 here is the fontsize
        }
        void update(){
            if (IsKeyPressed(KEY_P)){
                gameState = PLAYING;
            }
        }
};
class GameOver : public State{
    private:

    public:
        GameOver(GameState& gameState) : State(gameState) {}

        void draw(){
            
        }
        void update(){
            if (IsKeyPressed(KEY_ENTER)){
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
        Color     color     {255, 203, 0, 23};

        Menu     menu       {gameState};
        Play     play       {gameState};
        Help     help       {gameState};
        Settings settings   {gameState};
        Playing  playing    {gameState, color};
        Paused   paused     {gameState};
        Score    score      {gameState, playing};
        GameOver gameOver   {gameState};


    public:
        Game(){
            InitAudioDevice();
        }
        ~Game(){
            CloseAudioDevice();
        }

        void draw(){
            // clearing background
            ClearBackground(BLANK);

            switch(gameState)
            {
                case MENU:     { menu.draw();     break; }
                case PLAY:     { play.draw();     break; }
                case SETTINGS: { settings.draw(); break; }
                case HELP:     { help.draw();     break; }

                case PLAYING:  { playing.draw();  break; }
                case SCORE:     { score.draw();     break; }
                case PAUSED:   { paused.draw();   break; }
                case GAMEOVER: { gameOver.draw(); break; }
            }
        }
        void update(){
            switch(gameState)
            {
                case MENU:     { menu.update();     break; }
                case PLAY:     { play.update();     break; }
                case SETTINGS: { settings.update(); break; }
                case HELP:     { help.update();     break; }

                case PLAYING:  { playing.update();  break; }
                case SCORE:    { score.update();    break; }
                case PAUSED:   { paused.update();   break; } 
                case GAMEOVER: { gameOver.update(); break; }
            }
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
    // SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE); // op :D

    Image icon = LoadImage("Assets/Favicon/2.png");
    if (icon.data){
        SetWindowIcon(icon);
        UnloadImage(icon);
    }

    // const int ballRadius    {35};
    // const int ballBaseSpeed {10};
    // Ball ball(ballRadius, ballBaseSpeed, GOLD, MAROON);

    // const int paddleVelocityY  {11};
    // const int paddleWidth      {23};
    // const int paddleHeight    {135};
    // const int paddleRoundness  {50};
    // Paddle player(paddleVelocityY, paddleWidth, paddleHeight, paddleRoundness, GOLD);

    // const int computerPaddleVelocityY   {9};
    // const int computerPaddleWidth      {23};
    // const int computerPaddleHeight    {135};
    // const int computerPaddleRoundness  {50};
    // ComputerPaddle computer(computerPaddleVelocityY, computerPaddleWidth, computerPaddleHeight, computerPaddleRoundness, GOLD);

    // Color transparentGOLD = Color{255, 203, 0, 23};         // a lighter version of the defined GOLD color

    // Game game(ball, player, computer, transparentGOLD);

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

    CloseWindow();
} 