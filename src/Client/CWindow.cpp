#include "pch.h"
#include "CWindow.h"
#include "Entity.h"

CWindow::CWindow() : mpWindow(nullptr)
{
}

CWindow::~CWindow()
{
}

void CWindow::Initialize(unsigned int width, unsigned int height, const char* title)
{
	mpWindow = new sf::RenderWindow(sf::VideoMode(width, height), title);
}

void CWindow::Clear()
{
	mpWindow->clear(sf::Color::Black);
}

void CWindow::Draw(Entity* entity)
{
	mpWindow->draw(entity->GetDrawable());
}

void CWindow::Display()
{
	mpWindow->display();
}

bool CWindow::IsOpen()
{
	return mpWindow->isOpen();
}

void CWindow::HandleEvent()
{
	sf::Event event;
	while (mpWindow->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			mpWindow->close();
	}
}

sf::RenderWindow& CWindow::GetWindow()
{
	return *mpWindow;
}