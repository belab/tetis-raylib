#include "raylib.h"
#include <array>
#include <chrono>
#include <string>

const int M = 20;
const int N = 10;

int field[M][N] = {0};
struct Point {
	int x{}, y{};
};

struct Tetromino {
	explicit Tetromino(int shapeNr) {
		const int figures[7][4] = {
			1, 3, 5, 7, // I     0 1
			2, 4, 5, 7, // Z     2 3
			3, 5, 4, 6, // S     4 5
			3, 5, 4, 7, // T     6 7
			2, 3, 5, 7, // L
			3, 5, 7, 6, // J
			2, 3, 4, 5, // O
		};
		for (int i = 0; i < 4; i++)	{
			shape[i].x = figures[shapeNr][i] % 2; // 1 1 1 1
			shape[i].y = figures[shapeNr][i] / 2; // 0 1 2 3
		}
	}

	bool check() {
		for (auto& point : shape) {
			if (point.x < 0 || point.x >= N || point.y >= M)
				return false;
			else if (field[point.y][point.x] > 0)
				return false;
		}
		return true;
	};

	void moveAlongX(int dx){
		auto backup = shape;
		for (auto &point : shape) {
			point.x += dx;
		}
		if (!check())
			shape = backup;
	}
	void rotate(){
		auto backup = shape;
		Point center = shape[1]; // center of rotation
		for (auto &point : shape) {
			int x = point.y - center.y;
			int y = point.x - center.x;
			point.x = center.x - x;
			point.y = center.y + y;
		}
		if (!check())
			shape = backup;
	}

	bool moveDown(){
		auto backup = shape;
		for (auto &point : shape) {
			point.y += 1; // move down
		}
		if(!check()) {
			shape = backup;
			return false;
		}
		return true;
	}

	void printToField(int colorNum) {
		for (auto &point : shape)
			field[point.y][point.x] = colorNum;
	}

	std::array<Point, 4> shape;
};


int main(void)
{
	srand(124);

	InitWindow(360, 360, "TETRIS-raylib");
	SetTargetFPS(60);
	auto font = LoadFont("lcd.14.otf");
	auto texture = LoadTexture("images/tiles.png");

	int colorNum = 1;
	float timer = 0.01f, delay = 0.0f;
	auto startTime = std::chrono::high_resolution_clock::now();
	Tetromino a{colorNum-1};
	int scoreValue = 0;

	while (!WindowShouldClose())
	{
		auto stopTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();
		startTime = std::chrono::high_resolution_clock::now();
		timer+=time;

		int dx = 0;
		bool rotate = false;
		if (IsKeyPressed(KEY_UP))
			rotate = true;
		if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT))
			dx = -1;
		if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT))
			dx = 1;
		if (IsKeyDown(KEY_DOWN))
			delay = 0.05f;

		a.moveAlongX(dx);
		if (rotate)
			a.rotate();

		// move down
		if (timer > delay) {
			if(!a.moveDown()){
				a.printToField(colorNum);
				colorNum = 1 + rand() % 7;
				a = Tetromino{colorNum-1};
			}
			timer = 0;
		}

		// check and clear lines
		int k = M - 1;
		for (int i = M - 1; i > 0; i--) {
			int count = 0;
			for (int j = 0; j < N; j++) {
				if (field[i][j] > 0)
					count++;
				field[k][j] = field[i][j];
			}
			if (count < N)
				k--;
			else
				scoreValue+=100;
		}

		dx = 0;
		rotate = 0;
		delay = 0.3f;

		BeginDrawing();

			ClearBackground(BLACK);
			DrawTextEx(font, std::to_string(scoreValue).c_str(), {220.f, 0.f}, 30.f, 0.f, WHITE);
			DrawRectangle(180, 0, 2, 360, WHITE);
			for (int i = 0; i < M; i++) {
				for (int j = 0; j < N; j++)
				{
					if (field[i][j] == 0)
						continue;
					DrawTextureRec(texture, {field[i][j] * 18.f, 0, 18.f, 18.f}, {j * 18.f, i * 18.f}, WHITE);
				}
			}

			for (int i = 0; i < 4; i++)	{
				DrawTextureRec(texture, {colorNum * 18.f, 0, 18.f, 18.f}, {a.shape[i].x * 18.f, a.shape[i].y * 18.f}, WHITE);
			}

		EndDrawing();
	}

	CloseWindow();

	return 0;
}