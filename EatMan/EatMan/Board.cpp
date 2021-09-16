#include "Board.h"

Board::Board(float sizeX, float sizeY, float shapeSize, float shapeDelta, Color color, bool outline, int outlineThickness) : waves{} {
	float yDelta = sin(0.523599) * shapeSize;
	float xDelta = shapeSize - sin(1.0472) * shapeSize;

	size = Vector2f(sizeX, sizeY);
	gridSize = Vector2f(size.x / ((shapeSize + shapeDelta - xDelta) * 2) + 1 + WAVETRAIL * 2, size.y / ((shapeSize + shapeDelta/2 - yDelta) * 2) + 1);
	this->color = color;
	this->shapeSize = shapeSize;
	this->shapeDelta = shapeDelta;
	this->outline = outline;

	for (int y = 0; y < gridSize.y; y++)
	{
		for (int x = 0; x < gridSize.x; x++)
		{
			shapes.push_back(CircleShape(shapeSize, 6));
			if (outline) {
				shapes.back().setFillColor(Color::Transparent);
				shapes.back().setOutlineThickness(outlineThickness);
				shapes.back().setOutlineColor(color);
			}
			else {
				shapes.back().setFillColor(color);
			}

			shapes.back().setOrigin(shapeSize, shapeSize);
			shapes.back().setPosition(x * (shapeSize * 2 - xDelta + shapeDelta) + (shapeSize + shapeDelta/2) * (y % 2), y * (shapeSize * 2 - yDelta + shapeDelta));
		}
	}

	for (int i = 0; i < MAXWAVES; i++)
	{
		waves[i] = new struct Wave(-1, Color::Transparent);
		impact[i] = new struct Impact(0, 0, 0, Color::Transparent);
	}


	waveFrame = 0;
}

Board::~Board() {
	for (struct Wave* wave : waves) {
		delete wave;
	}
}

void Board::Draw(RenderWindow* window) {
	for (list<CircleShape>::iterator shape = shapes.begin(); shape != shapes.end(); shape++)
	{
		window->draw( (*shape) );
	}

	for (int i = 0; i < MAXWAVES; i++)
	{
		if (impact[i]->active) {
			//window->draw(impact[i]->debug);
		}
	}
}

void Board::Act(float elapsedTime) {
	if (elapsedTime - waveFrame > WAVESPEED) {
		//ImpactMovement();
		WaveMovement();
		waveFrame = elapsedTime;
	}
}

void Board::WaveMovement() {
	
	for (int i = 0; i < MAXWAVES; i++)
	{
		if (waves[i]->index != -1) {
			list<CircleShape>::iterator it = shapes.begin();
			advance(it, waves[i]->index);

			int trail = 0;
			for (int delta = 0; delta < WAVETRAIL; delta++)
			{
				if (waves[i]->index - delta >= 0) {
					trail = delta;
				}
			}

			for (int j = 0; j < gridSize.y; j++)
			{
				advance(it, -trail);

				// Trail
				for (int delta = 0; delta < trail; delta++)
				{
					Color color = waves[i]->color;
					color = Color(
						this->color.r + (color.r - this->color.r) * (WAVETRAIL - (trail - delta)) / (WAVETRAIL + 2),
						this->color.g + (color.g - this->color.g) * (WAVETRAIL - (trail - delta)) / (WAVETRAIL + 2),
						this->color.b + (color.b - this->color.b) * (WAVETRAIL - (trail - delta)) / (WAVETRAIL + 2),
						color.a
					);

					SetShapeColor(&(*it), color);
					advance(it, 1);
				}

				// Wave
				SetShapeColor(&(*it), waves[i]->color);

				if (distance(it, shapes.end()) > gridSize.x) {
					advance(it, gridSize.x);
				}
			}

			waves[i]->index++;
			if (waves[i]->index >= gridSize.x) {
				waves[i]->index = -1;
			}
		}
	}
}

void Board::Wave(Color color) {
	for (int i = 0; i < MAXWAVES; i++)
	{
		if(waves[i]->index == -1) {
			waves[i]->index = 0;
			waves[i]->color = color;
			return;
		}
	}
}

void Board::ImpactMovement() {
	for (int i = 0; i < MAXWAVES; i++)
	{
		if (impact[i]->active) {
			Color color = impact[i]->color;
			color = Color(
				this->color.r + (color.r - this->color.r) * (IMPACTMAXRADIUS - impact[i]->radius/(shapeSize*2)) / IMPACTMAXRADIUS,
				this->color.g + (color.g - this->color.g) * (IMPACTMAXRADIUS - impact[i]->radius / (shapeSize * 2)) / IMPACTMAXRADIUS,
				this->color.b + (color.b - this->color.b) * (IMPACTMAXRADIUS - impact[i]->radius / (shapeSize * 2)) / IMPACTMAXRADIUS,
				color.a
			);
			float cx = impact[i]->center.x, cy = impact[i]->center.y;
			for (CircleShape &shape : shapes)
			{
				float shpx = shape.getPosition().x, shpy = shape.getPosition().y;
				float dist = sqrt((shpx - cx) * (shpx - cx) + (shpy - cy) * (shpy - cy));
				if (impact[i]->radius > dist) {
					SetShapeColor(&shape, impact[i]->color);
					/*if (impact[i]->radius - (shapeSize * 2.0f) > dist) {
						SetShapeColor(&shape, color);
					}*/
				}
			}
			impact[i]->radius += (shapeSize + shapeDelta) * 2;
			impact[i]->debug.setRadius(impact[i]->radius);
			impact[i]->debug.setOrigin(impact[i]->radius, impact[i]->radius);
			//impact[i]->color = Color(impact[i]->color.r / 2, impact[i]->color.g / 2, impact[i]->color.b / 2, impact[i]->color.a);
			impact[i]->color = color;
			if (impact[i]->radius >= IMPACTMAXRADIUS * shapeSize * 2) {
				impact[i]->active = false;
			}
		}
	}
}

void Board::Impact(float posX, float posY, Color color) {
	for (int i = 0; i < MAXWAVES; i++)
	{
		if (!impact[i]->active) {
			/*int y = int(posY / (shapeSize + shapeDelta));
			int x = int((posX + shapeSize * ((y + 1) % 2)) / (shapeSize * 2 + 2 * (shapeSize - sin(1.0472) * shapeSize) + 1));
			int shapeIndex = x + y * (gridSize.x + 1);
			cout << shapeIndex << " = " << x << " . " << y << endl;
			list<CircleShape>::iterator it = shapes.begin();
			advance(it, shapeIndex);*/

			//impact[i]->center = Vector2f((*it).getPosition().x, (*it).getPosition().y);
			impact[i]->center = Vector2f(posX, posY);
			impact[i]->color = color;
			impact[i]->active = true;
			impact[i]->radius = shapeSize * 2;

			impact[i]->debug.setRadius(impact[i]->radius);
			impact[i]->debug.setOrigin(shapeSize, shapeSize);
			impact[i]->debug.setPosition(impact[i]->center.x, impact[i]->center.y);
			impact[i]->debug.setFillColor(Color::Transparent);
			impact[i]->debug.setOutlineThickness(3);
			impact[i]->debug.setOutlineColor(Color::Green);
			return;
		}
	}
}

void Board::SetShapeColor(CircleShape* shape, Color color) {
	if (outline) {
		shape->setOutlineColor(color);
	}
	else {
		shape->setFillColor(color);
	}
}

Color Board::GetShapeColor(CircleShape* shape) {
	if (outline) {
		return shape->getOutlineColor();
	}
	else {
		return shape->getFillColor();
	}
}

Color Board::GetColor() {
	return color;
}

Vector2f Board::GetSize() {
	return size;
}

Vector2f Board::GetGridSize() {
	return gridSize;
}

float Board::GetShapeSize() {
	return shapeSize;
}

list<CircleShape>* Board::GetShapes() {
	return &shapes;
}