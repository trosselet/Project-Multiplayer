#ifndef CWINDOW_H
#define CWINDOW_H

class Entity;

class CWindow
{
private:
	sf::RenderWindow* mpWindow;
public:
	CWindow();
	virtual ~CWindow();

	void Initialize(unsigned int width, unsigned int height, const char* title);
	void Clear();
	void Draw(Entity* entity);

	template<typename T>
	void DrawShape(T& drawable);

	void Display();
	bool IsOpen();
	void HandleEvent();

	sf::RenderWindow& GetWindow();
};

template<typename T>
void CWindow::DrawShape(T& drawable)
{
	static_assert(std::is_base_of<sf::Drawable, T>::value, "T must derive from sf::Drawable");
	mpWindow->draw(drawable);
}

#endif

