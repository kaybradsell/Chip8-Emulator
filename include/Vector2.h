#pragma once

#define NORTH Vector2Int(0, -1)
#define SOUTH Vector2Int(0, 1)
#define WEST Vector2Int(-1, 0)
#define EAST Vector2Int(1, 0)

struct Vector2Int
{
	int x, y;

	Vector2Int()
	{
		x = 0;
		y = 0;
	}

	Vector2Int(int xx, int yy)
	{
		x = xx;
		y = yy;
	}

	Vector2Int(const Vector2Int& other)
	{
		this->x = other.x;
		this->y = other.y;
	}

	void Add(const Vector2Int& other)
	{
		this->x += other.x;
		this->y += other.y;
	}

	bool operator==(const Vector2Int& other) const
	{
		bool sameX = (this->x == other.x);
		bool sameY = (this->y == other.y);

		return (sameX && sameY);
	}

	void Randomise(int limit)
	{
		this->x = rand() % limit;
		this->y = rand() % limit;
	}
};