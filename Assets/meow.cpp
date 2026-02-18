#include <string>
#include <raylib.h>

class Ball{
    private:
        const int screenWidth;
        const int screenHeight;
        const int radius;
        int positionX;
        int positionY;
        int velocityX;
        int velocityY;
        const Color innerColor;
        const Color outerColor;

    public:
        Ball(const int screenWidth, const int screenHeight, const int radius, const int velocityX, const int velocityY, const Color innerColor, const Color outerColor)
        : screenWidth(screenWidth), screenHeight(screenHeight), positionX(screenWidth / 2), positionY(screenHeight / 2), radius(radius), velocityX(velocityX), velocityY(velocityY), innerColor(innerColor), outerColor(outerColor) {
            this->velocityX *= (GetRandomValue(0, 1)? 1 : -1);
            this->velocityY *= (GetRandomValue(0, 1)? 1 : -1);      // to randomize directions
        }

        void reset(){
            this->positionX = screenWidth / 2;
            this->positionY = screenHeight / 2;

            // to randomize directions
            this->velocityX *= (GetRandomValue(0, 1)? 1 : -1);
            this->velocityY *= (GetRandomValue(0, 1)? 1 : -1);
        }
        void update(){
            this->positionX += this->velocityX;
            if (((this->positionX - this->radius) <= 0) || ((this->positionX + this->radius) >= this->screenWidth)){
                this->velocityX *= -1;                              // reversing component's direction vector
            }

            this->positionY += this->velocityY;
            if (((this->positionY - this->radius) <= 0) || ((this->positionY + this->radius) >= this->screenHeight)){
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
        }
};
class Paddle{
    protected:
        const int screenWidth;
        const int screenHeight;
        int positionX;
        int positionY;
        int velocityY;
        int width;
        int height;
        const int roundness;
        const Color color;
        Rectangle rectangle;
    
    public:
        Paddle(const int screenWidth, const int screenHeight, const int velocityY, const int width, const int height, const int roundness, const Color color)
        : screenWidth(screenWidth), screenHeight(screenHeight), width(width), height(height), roundness(roundness), velocityY(velocityY), color(color) {
            this->positionX = (5 + this->width / 2);        // 5 = dist from edge of screen (ie offset)
            this->positionY = (this->screenHeight / 2 - this->height / 2);
            this->rectangle = Rectangle{(float) this->positionX, (float) this->positionY, (float) this->width, (float) this->height};
        }

        void update(){
            // move up
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_LEFT)){
                if ((this->positionY - this->velocityY) >= 0){
                    this->positionY  -= this->velocityY;
                    this->rectangle.y = this->positionY;
                }
            }
            
            // move down
            else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_D) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_RIGHT)){
                if (((this->positionY + this->height) + this->velocityY) <= this->screenHeight){
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
            this->positionX = (5 + this->width / 2);        // 5 = dist from edge of screen (ie offset)
            this->positionY = (this->screenHeight / 2 - this->height / 2);
            this->rectangle = Rectangle{(float) this->positionX, (float) this->positionY, (float) this->width, (float) this->height};
        }
};
class ComputerPaddle : public Paddle{
    public:
        ComputerPaddle(const int screenWidth, const int screenHeight, const int velocityY, const int width, const int height, const int roundness, const Color color)
        : Paddle(screenWidth, screenHeight, velocityY, width, height, roundness, color){
            this->positionX = (this->screenWidth - this->width - (5 + this->width / 2));            // 5 + width/2 = offset: ie dist from edge of screen
            this->rectangle.x = this->positionX;
        }

        void update(Vector2 ballCenter, int ballRadius){
            if (ballCenter.y < this->positionY){
                this->positionY -= this->velocityY;
                if (this->positionY < 0){ this->positionY = 0; }

                this->rectangle.y = this->positionY;
            }

            else if (ballCenter.y > (this->positionY + this->height)){
                this->positionY += this->velocityY;
                if ((this->positionY + this->height) > this->screenHeight){ this->positionY = this->screenHeight; }

                this->rectangle.y = this->positionY;
            }
        }
        void reset(){
            this->positionX = (this->screenWidth - this->width - (5 + this->width / 2));            // 5 + width/2 = offset: ie dist from edge of screen
            this->positionY = (this->screenHeight / 2 - this->height / 2);
            this->rectangle = Rectangle{(float) this->positionX, (float) this->positionY, (float) this->width, (float) this->height};
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

        const int screenWidth;
        const int screenHeight;
        int playerScore;
        int computerScore;
        const Color mainColor;
        const Color lightColor;
        const int fontSize;

        int elapsedTime;
        bool isPaused;
        int pauseStartTime;
        const float pauseDuration;

        Sound ballHitSFX;
        Sound playerScoreSFX;
        Sound computerScoreSFX;
        Sound powerUpSFX;

    public:
        Game(Ball& ball, Paddle& player, ComputerPaddle& computer, const Color mainColor, const Color lightColor)
        : ball(ball), player(player), computer(computer), mainColor(mainColor), lightColor(lightColor), screenWidth(GetScreenWidth()), screenHeight(GetScreenHeight()), playerScore(0), computerScore(0), fontSize(300), elapsedTime(0), isPaused(false), pauseStartTime(0), pauseDuration(3) {
            InitAudioDevice();

            this->ballHitSFX       = LoadSound("Assets/SFX/ballHit.mp3");
            this->playerScoreSFX   = LoadSound("Assets/SFX/playerScore.mp3");
            this->computerScoreSFX = LoadSound("Assets/SFX/computerScore.mp3");
            this->powerUpSFX       = LoadSound("Assets/SFX/powerUp.mp3");
        }
        ~Game(){
            UnloadSound(this->ballHitSFX);
            UnloadSound(this->playerScoreSFX);
            UnloadSound(this->computerScoreSFX);
            UnloadSound(this->powerUpSFX);

            CloseAudioDevice();
        }

        void reset(){
            this->ball.reset();
            this->player.reset();
            this->computer.reset();

            this->isPaused = true;
            this->pauseStartTime = GetTime();
        }
        void update(){
            if (!this->isPaused){
                // ball
                this->ball.update();

                // paddle update + collision

                    // player paddle
                    this->player.update();            
                    if (CheckCollisionCircleRec(this->ball.getCenter(), this->ball.getRadius(), this->player.getRectangle())){
                        this->ball.reverseVelocityX();
                        PlaySound(this->ballHitSFX);
                    }

                    // computer paddle
                    this->computer.update(this->ball.getCenter(), this->ball.getRadius());
                    if (CheckCollisionCircleRec(this->ball.getCenter(), this->ball.getRadius(), this->computer.getRectangle())){
                        this->ball.reverseVelocityX();
                        PlaySound(this->ballHitSFX); 
                    }

                // scoring

                    // player scores
                    if ((this->ball.getCenter().x + this->ball.getRadius()) >= this->screenWidth){
                        this->playerScore++;
                        PlaySound(this->playerScoreSFX);

                        reset();
                    }

                    // computer scores
                    if ((this->ball.getCenter().x - this->ball.getRadius()) <= 0){
                        this->computerScore++;
                        PlaySound(this->computerScoreSFX);

                        reset();
                    }
            }

            // elapsed time
            this->elapsedTime = GetTime();
        }
        void draw(){
            // clearing background
            ClearBackground(BLANK);

            // centre divider/line + circle
            DrawRectangle(this->screenWidth / 2 - 5, 0, 10, this->screenHeight / 2 - this->screenWidth / 7, this->lightColor);
            DrawCircle(this->screenWidth / 2, this->screenHeight / 2, this->screenWidth / 7, this->lightColor);
            DrawRectangle(this->screenWidth / 2 - 5, this->screenHeight / 2 + this->screenWidth / 7, 10, this->screenHeight, this->lightColor);

            // scores
            DrawText(TextFormat("%d", this->playerScore),       this->screenWidth / 4 - MeasureText(TextFormat("%d", this->playerScore), this->fontSize) / 2,   this->screenHeight / 2 - this->fontSize / 2, this->fontSize, this->lightColor);
            DrawText(TextFormat("%d", this->computerScore), 3 * this->screenWidth / 4 - MeasureText(TextFormat("%d", this->computerScore), this->fontSize) / 2, this->screenHeight / 2 - this->fontSize / 2, this->fontSize, this->lightColor);

            // time elapsed
            if (!isPaused){
                DrawText(TextFormat("%d", this->elapsedTime), this->screenWidth / 2 - MeasureText(TextFormat("%d", this->elapsedTime), this->fontSize / 1.5) / 2, this->screenHeight / 2 - this->fontSize / 3, this->fontSize / 1.5, BLACK);
            }

            this->ball.draw();
            this->player.draw();
            this->computer.draw();

            if (this->isPaused){
                DrawText(TextFormat("Paused: %d", 3 - (this->elapsedTime - this->pauseStartTime)), 5, this->screenHeight - this->fontSize / 8 - 5, this->fontSize / 8, this->lightColor);

                if ((this->elapsedTime - this->pauseStartTime) >= this->pauseDuration){
                    this->isPaused = false;
                    this->pauseStartTime = 0;
                }
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

    Image icon = LoadImage("Assets/Favicon/2.png");
    if (icon.data){
        SetWindowIcon(icon);
        UnloadImage(icon);
    }

    const int ballRadius    {35};
    const int ballVelocityX {13};
    const int ballVelocityY {13};
    Ball ball(screenWidth, screenHeight, ballRadius, ballVelocityX, ballVelocityY, GOLD, MAROON);

    const int paddleVelocityY  {11};
    const int paddleWidth      {23};
    const int paddleHeight    {123};
    const int paddleRoundness  {50};
    Paddle player(screenWidth, screenHeight, paddleVelocityY, paddleWidth, paddleHeight, paddleRoundness, GOLD);

    const int computerPaddleVelocityY  {10};
    const int computerPaddleWidth      {23};
    const int computerPaddleHeight    {123};
    const int computerPaddleRoundness  {50};
    ComputerPaddle computer(screenWidth, screenHeight, computerPaddleVelocityY, computerPaddleWidth, computerPaddleHeight, computerPaddleRoundness, GOLD);
    
    Color transparentGOLD = Color{255, 203, 0, 23};

    Game game(ball, player, computer, GOLD, transparentGOLD);

    
    while (!WindowShouldClose()){
        game.update();
    
        BeginDrawing();
            game.draw();
        EndDrawing();
    }

    CloseWindow();
}