#include <string>
#include <raylib.h>

/*
    TODO
        - powerups
        - menu
        - gamepause
        - win condition
        - replay
*/

enum GameState{
    // mainMenu
    MENU      = 1,      // main menu screen
    PLAY      = 2,      // - play menu
    SETTINGS  = 3,      // - setting menu
    HELP      = 4,      // - help menu

    // gameOn
    PLAYING   = 5,      // game playing
    WAIT      = 6,      // wait after score
    PAUSED    = 7,      // game paused
    GAMEOVER  = 8       // gameover 
};
enum LastScorer{
    PLAYER, 
    COMPUTER
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
            this->velocityX = (this->baseSpeed * (GetRandomValue(0, 1) ? 1 : -1));
            this->velocityY = (this->baseSpeed * (GetRandomValue(0, 1) ? 1 : -1));
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
            this->positionX += ((this->velocityX > 0)? 23 : -23);
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
class Game{
    private:
        Ball& ball;
        Paddle& player;
        ComputerPaddle& computer;

        const int fontSize;
        const Color color;

        GameState gameState;
        int elapsedTime;
        int playerScore;
        int computerScore;
        LastScorer lastScorer;

        int pauseStartTime;
        const int pauseDuration;

        Sound ballHitSFX;
        Sound playerScoreSFX;
        Sound computerScoreSFX;
        // Sound powerUpSFX;

    public:
        Game(Ball &ball, Paddle &player, ComputerPaddle &computer, const Color color)
        : ball(ball), player(player), computer(computer), color(color), playerScore(0), computerScore(0), fontSize(300), gameState(MENU), elapsedTime(0), pauseStartTime(0), pauseDuration(3)
        {
            InitAudioDevice();

            this->ballHitSFX       = LoadSound("Assets/SFX/ballHit.mp3");
            this->playerScoreSFX   = LoadSound("Assets/SFX/playerScore.mp3");
            this->computerScoreSFX = LoadSound("Assets/SFX/computerScore.mp3");
            // this->powerUpSFX       = LoadSound("Assets/SFX/powerUp.mp3");
        }
        ~Game(){
            UnloadSound(this->ballHitSFX);
            UnloadSound(this->playerScoreSFX);
            UnloadSound(this->computerScoreSFX);
            // UnloadSound(this->powerUpSFX);

            CloseAudioDevice();
        }

        void reset(){
            this->ball.reset();
            this->player.reset();
            this->computer.reset();
        }
        void drawMenu(){

        }
        void updateMenu(){

        }
        void drawPlay(){

        }
        void drawSettings(){

        }
        void drawHelp(){

        }
        void drawPlaying(){
            // centre divider/line + circle
            DrawRectangle(GetScreenWidth() / 2 - 5, 0, 10, GetScreenHeight() / 2 - GetScreenWidth() / 7, this->color);
            DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, GetScreenWidth() / 7, this->color);
            DrawRectangle(GetScreenWidth() / 2 - 5, GetScreenHeight() / 2 + GetScreenWidth() / 7, 10, GetScreenHeight(), this->color);

            // scores
            DrawText(TextFormat("%d", this->playerScore),       GetScreenWidth() / 4 - MeasureText(TextFormat("%d", this->playerScore),   this->fontSize) / 2, GetScreenHeight() / 2 - this->fontSize / 2, this->fontSize, this->color);
            DrawText(TextFormat("%d", this->computerScore), 3 * GetScreenWidth() / 4 - MeasureText(TextFormat("%d", this->computerScore), this->fontSize) / 2, GetScreenHeight() / 2 - this->fontSize / 2, this->fontSize, this->color);

            // elapsed time
            DrawText(TextFormat("%d", this->elapsedTime), GetScreenWidth() / 2 - MeasureText(TextFormat("%d", this->elapsedTime), this->fontSize / 1.5) / 2, GetScreenHeight() / 2 - this->fontSize / 3, this->fontSize / 1.5, BLACK);
        
            // drawing objects
            this->ball.draw();
            this->player.draw();
            this->computer.draw();
        }
        void updatePlaying(){
            // updating ball
            this->ball.update();

            // updating paddles
            this->player.update();
            this->computer.update(this->ball.getCenter(), this->ball.getRadius());

            // checking for paddle+ball collision
                // player paddle
                if (CheckCollisionCircleRec(this->ball.getCenter(), this->ball.getRadius(), this->player.getRectangle())){
                    this->ball.reverseVelocityX();
                    PlaySound(this->ballHitSFX);
                }

                // computer paddle
                if (CheckCollisionCircleRec(this->ball.getCenter(), this->ball.getRadius(), this->computer.getRectangle())){
                    this->ball.reverseVelocityX();
                    PlaySound(this->ballHitSFX);
                }

            // scoring
                // player scores
                if ((this->ball.getCenter().x + this->ball.getRadius()) >= GetScreenWidth()){
                    this->playerScore++;
                    PlaySound(this->playerScoreSFX);

                    reset();
                    this->lastScorer = PLAYER;
                    this->gameState = WAIT;
                    this->pauseStartTime = GetTime();
                }

                // computer scores
                if ((this->ball.getCenter().x - this->ball.getRadius()) <= 0){
                    this->computerScore++;
                    PlaySound(this->computerScoreSFX);

                    reset();
                    this->lastScorer = COMPUTER;
                    this->gameState = WAIT;
                    this->pauseStartTime = GetTime();
                }
            
            // pausing game if key 'p' pressed
            if (IsKeyPressed(KEY_P)){
                this->gameState = PAUSED;
            }
        }
        void drawPaused(){
            const int textSize {83};
            DrawText(TextFormat("GamePlay Paused.\nPress \"p\" to resume."), GetScreenWidth() / 2 - MeasureText(TextFormat("GamePlay Paused.\nPress 'p' to resume."), textSize) / 2, GetScreenHeight() / 2 - textSize, textSize, Color{this->color.r, this->color.g, this->color.b, 83});        // thats long....;  100 here is the fontsize
        }
        void updatePaused(){
            if (IsKeyPressed(KEY_P)){
                this->gameState = PLAYING;
            }
        }
        void drawWait(){
            // 'who scored' text
            const int textSize {83};
            DrawText(TextFormat("%s Scored!", (this->lastScorer == PLAYER)? "You" : "Computer"), GetScreenWidth() / 2 - MeasureText(TextFormat("%s Scored!", (this->lastScorer == PLAYER)? "You" : "Computer"), textSize)/2, GetScreenHeight()/2 - textSize/2, textSize, Color{this->color.r, this->color.g, this->color.b, 83});        // thats long....;  100 here is the fontsize
            
            // 'resuming in' text
            DrawText(TextFormat("(Game Resuming in %d...)", this->pauseDuration - (this->elapsedTime - this->pauseStartTime)), 5, GetScreenHeight() - this->fontSize / 8 - 5, this->fontSize / 8, Color{this->color.r, this->color.g, this->color.b, 83});
        }
        void updateWait(){
            if ((this->elapsedTime - this->pauseStartTime) >= this->pauseDuration){
                this->gameState = PLAYING;
                this->pauseStartTime = 0;
            }
        }
        void drawGameOver(){
            
        }
        void draw(){
            // clearing background
            ClearBackground(BLANK);

            switch(this->gameState)
            {
                // case MENU:     { drawMenu();     break; }
                // case PLAY:     { drawPlay();     break; }
                // case SETTINGS: { drawSettings(); break; }
                // case HELP:     { drawHelp();     break; }
                case PLAYING:  { drawPlaying();  break; }
                case PAUSED:   { drawPaused();   break; }
                case WAIT:     { drawWait();     break; }
                // case GAMEOVER: { drawGameOver(); break; }
            }
        }
        void update(){
            switch(this->gameState)
            {
                // case MENU:     { updateMenu();     break; }
                // case PLAY:     { updatePlay();     break; }
                // case SETTINGS: { updateSettings(); break; }
                // case HELP:     { updateHelp();     break; }
                case PLAYING:  { updatePlaying();  break; }
                case PAUSED:   { updatePaused();   break; } 
                case WAIT:     { updateWait();     break; }
                // case GAMEOVER: { updateGameOver(); break; }
            }

            // updating elapsed time
            this->elapsedTime = GetTime();
        }
};

int main()
{
    const int screenWidth  {1300};
    const int screenHeight  {700};

    InitWindow(screenWidth, screenHeight, "Mr. Pong 🏓");
    SetWindowOpacity(0.8);
    SetTargetFPS(63);

    Image icon = LoadImage("Assets/Favicon/2.png");
    if (icon.data){
        SetWindowIcon(icon);
        UnloadImage(icon);
    }

    const int ballRadius    {35};
    const int ballBaseSpeed {10};
    Ball ball(ballRadius, ballBaseSpeed, GOLD, MAROON);

    const int paddleVelocityY  {11};
    const int paddleWidth      {23};
    const int paddleHeight    {135};
    const int paddleRoundness  {50};
    Paddle player(paddleVelocityY, paddleWidth, paddleHeight, paddleRoundness, GOLD);

    const int computerPaddleVelocityY   {9};
    const int computerPaddleWidth      {23};
    const int computerPaddleHeight    {135};
    const int computerPaddleRoundness  {50};
    ComputerPaddle computer(computerPaddleVelocityY, computerPaddleWidth, computerPaddleHeight, computerPaddleRoundness, GOLD);

    Color transparentGOLD = Color{255, 203, 0, 23};         // a lighter version of the defined GOLD color

    Game game(ball, player, computer, transparentGOLD);

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