
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

void MoveCrossbow(PhysicsSprite& crossbow, int elapsedMS) {
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        Vector2f newPos(crossbow.getCenter());
        newPos.x = newPos.x + (KB_SPEED * elapsedMS);
        crossbow.setCenter(newPos);
    }
    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        Vector2f newPos(crossbow.getCenter());
        newPos.x = newPos.x - (KB_SPEED * elapsedMS);
        crossbow.setCenter(newPos);
    }
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "Break Out");
    World world(Vector2f(0, 0));
    int score(0);
    int balls(30);

    PhysicsSprite& crossBow = *new PhysicsSprite();
    Texture cbowTex;
    LoadTex(cbowTex, "images/crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter(Vector2f(400,
        600 - (sz.y / 2)));


    PhysicsSprite ball; //ball where arrow was
    Texture ballTex;
    LoadTex(ballTex, "images/ball.png");
    ball.setTexture(ballTex);
    bool drawingBall(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    PhysicsRectangle left;
    left.setSize(Vector2f(10, 600));
    left.setCenter(Vector2f(5, 300));
    left.setStatic(true);
    world.AddPhysicsBody(left);

    PhysicsRectangle right;
    right.setSize(Vector2f(10, 600));
    right.setCenter(Vector2f(795, 300));
    right.setStatic(true);
    world.AddPhysicsBody(right);

    Texture redTex;
    LoadTex(redTex, "images/duck.png");
    PhysicsShapeList<PhysicsSprite> ducks;
    for (int i(0); i < 6; i++) {
        PhysicsSprite& duck = ducks.Create();//.Sprite?
        duck.setTexture(redTex);
        int x = 50 + ((700 / 5) * i);
        Vector2f sz = duck.getSize();
        duck.setCenter(Vector2f(x, 20 + (sz.y / 2)));
        duck.setVelocity(Vector2f(0.25, 0));
        world.AddPhysicsBody(duck);
        duck.onCollision =
            [&drawingBall, &world, &ball, &duck, &ducks, &score, &right]
        (PhysicsBodyCollisionResult result) {
            if (result.object2 == ball) {
                drawingBall = false;
                world.RemovePhysicsBody(ball);
                world.RemovePhysicsBody(duck);
                ducks.QueueRemove(duck);
                score += 10;
            }
            else if (result.object2 == right) {
                drawingBall = false;
                world.RemovePhysicsBody(ball);
                world.RemovePhysicsBody(duck);
                ducks.QueueRemove(duck);
                score += 10;
            }
        };
    }

    top.onCollision = [&drawingBall, &world, &ball]
    (PhysicsBodyCollisionResult result) {
        drawingBall = false;
        world.RemovePhysicsBody(ball);
    };

    Text scoreText;
    Font font;
    if (!font.loadFromFile("arial.ttf")) {
        cout << "Couldn't load font arial.ttf" << endl;
        exit(1);
    }
    scoreText.setFont(font);
    Text ballCountText;
    ballCountText.setFont(font);

    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);
    bool win = false;
    while ((balls > 0) || drawingBall) {
        if (score == 60)
        {
            win = true;
            break;

        }
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        if (deltaMS > 9) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            MoveCrossbow(crossBow, deltaMS);
            if (Keyboard::isKeyPressed(Keyboard::Space) &&
                !drawingBall) {
                drawingBall = true;
                ball.setCenter(crossBow.getCenter());
                ball.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(ball);
                balls -= 1;
            }

            window.clear();
            if (drawingBall) {
                window.draw(ball);
            }
            for (PhysicsShape& duck : ducks) {
                window.draw((PhysicsSprite&)duck);
            }
            window.draw(crossBow);
            scoreText.setString(to_string(score));
            FloatRect textBounds = scoreText.getGlobalBounds();
            scoreText.setPosition(
                Vector2f(790 - textBounds.width, 590 - textBounds.height));
            window.draw(scoreText);
            ballCountText.setString(to_string(balls));
            textBounds = ballCountText.getGlobalBounds();
            ballCountText.setPosition(
                Vector2f(10, 590 - textBounds.height));
            window.draw(ballCountText);
            //world.VisualizeAllBounds(window);

            window.display();
            ducks.DoRemovals();
        }
    }
    Text gameOverText;
    gameOverText.setFont(font);
    if (win)
    {
        gameOverText.setString("YOU WIN");
    }
    else
    {
        gameOverText.setString("GAME OVER");
    }
    FloatRect textBounds = gameOverText.getGlobalBounds();
    gameOverText.setPosition(Vector2f(
        400 - (textBounds.width / 2),
        300 - (textBounds.height / 2)
    ));
    window.draw(gameOverText);
    window.display();
    while (true);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
